#include "collada.hpp"
#define MAX_FILE_SIZE 10000000


static const wchar_t *findTag(const wchar_t *tag, const wchar_t *text)
{
	size_t tagLength = wcslen(tag);
	while(*text != 0)
	{
		if(!wcsncmp(text, tag, tagLength))
			return text;
		text++;
	}
	return NULL;
}

static std::wstring_view getElementText(const wchar_t *text)
{
	const wchar_t *endOpeningTag = findTag(L">", text) + 1;
	if(endOpeningTag == 0)
		return {};
	const wchar_t *closingTag = findTag(L">", endOpeningTag) + 1;
	if(closingTag == 0)
		return {};
	return {endOpeningTag, (size_t)(closingTag - endOpeningTag)};
}

static void parseFloatArray(const std::wstring_view& text, NamedFloatArray& floatArray)
{
	const wchar_t *stringStart = &(*text.begin());
	const wchar_t *stringEnd = stringStart + text.length();
	while(stringStart < stringEnd)
	{
		wchar_t *nextChar;
		float f = std::wcstof(&(*stringStart), &nextChar);
		if(nextChar == stringEnd || nextChar == stringStart) break;
		floatArray.floats.push_back(f);
		stringStart = nextChar;
	}
}

static void parseIntArray(const std::wstring_view& text, TriangleGroup& tg)
{
	const wchar_t *stringStart = &(*text.begin());
	const wchar_t *stringEnd = stringStart + text.length();
	while(stringStart < stringEnd)
	{
		wchar_t *nextChar;
		float f = std::wcstof(&(*stringStart), &nextChar);
		if(nextChar == stringEnd || nextChar == stringStart) break;
		tg.elements.push_back((int)f);
		stringStart = nextChar;
	}
}

static void findSourceIds(const wchar_t *text, const wchar_t *textEnd, ColladaData& collada)
{
	const wchar_t *meshTag = text;
	while(true)
	{
		const wchar_t *sourceTag = findTag(L"<source", text);
		if(!sourceTag)
			break;
		const wchar_t *idAttribute = findTag(L"id=", sourceTag);
		if(!idAttribute)
			break;
		const wchar_t *idStart = idAttribute + 4;
		if(idStart >= textEnd)
			break;
		const wchar_t *idEnd = findTag(L"\"", idStart);
		if(!idEnd)
			break;
		if((size_t)(idEnd - idStart) <= 0)
			break;
		collada.ids.emplace_back();
		collada.ids.back().name = std::wstring_view(idStart, (size_t)(idEnd - idStart));
		text = idEnd;


		const wchar_t *floatArrayTag = findTag(L"<float_array", idEnd);
		if(!floatArrayTag)
			break;
		const std::wstring_view elementText = getElementText(floatArrayTag);
		if(elementText.data() == nullptr)
			break;
		parseFloatArray(elementText, collada.ids.back());
	}

	text = meshTag;
	while(true)
	{
		const wchar_t *vertTag = findTag(L"<vertices", text);
		if(!vertTag)
			break;
		VertexGroup vg = {};
		const wchar_t *vertTagIdStart = findTag(L"id=", vertTag) + 4;
		if((size_t)vertTagIdStart == 4)
			break;
		const wchar_t *vertTagIdEnd = findTag(L"\"", vertTagIdStart);
		if(!vertTagIdEnd)
			break;
		vg.name = std::wstring_view(vertTagIdStart, (size_t)(vertTagIdEnd - vertTagIdStart));

		while(true)
		{
			const wchar_t *inputTag = findTag(L"<input", vertTag);
			if(!inputTag)
				break;
			const wchar_t *semanticStart = findTag(L"semantic=", inputTag) + 10;
			if((size_t)vertTagIdStart == 10)
				break;
			const wchar_t *semanticEnd = findTag(L"\"", semanticStart);
			if(!semanticEnd)
				break;
			std::wstring_view semanticValue = {semanticStart, (size_t)(semanticEnd - semanticStart)};
			if(semanticValue != L"POSITION")
			{
				vertTag = semanticEnd;
				continue;
			}

			const wchar_t *sourceTagStart = findTag(L"source=", inputTag) + 9;
			if((size_t)sourceTagStart == 9)
				break;
			const wchar_t *sourceTagEnd = findTag(L"\"", sourceTagStart);
			if(!sourceTagEnd)
				break;
			vg.sourceId = {sourceTagStart, (size_t)(sourceTagEnd - sourceTagStart)};
			break;
		}
		collada.vertexGroups.push_back(vg);
		text = vertTagIdStart;

		while(true)
		{
			const wchar_t *triTag = findTag(L"<triangles", text);
			if(!triTag)
				break;
			const wchar_t *triClosingTag = findTag(L"</triangles", triTag);
			if(!triClosingTag)
				break;
			const wchar_t *materialTagStart = findTag(L"material=", text) + 10;
			if((size_t)materialTagStart == 10)
				break;
			const wchar_t *materialTagEnd = findTag(L"\"", materialTagStart);
			if(!materialTagEnd)
				break;
			std::wstring_view materialName = {materialTagStart, (size_t)(materialTagEnd - materialTagStart)};
			const wchar_t *originalTriTag = triTag;
			TriangleGroup tg = {};
			tg.materialName = materialName;
			while(true)
			{
				const wchar_t *inputTag = findTag(L"<input", triTag);
				if(!inputTag)
					break;
				if(inputTag > triClosingTag) break;
				const wchar_t *semanticStart = findTag(L"semantic=", inputTag) + 10;
				if((size_t)vertTagIdStart == 10)
					break;
				const wchar_t *semanticEnd = findTag(L"\"", semanticStart);
				if(!semanticEnd)
					break;
				std::wstring_view semanticValue = {semanticStart, (size_t)(semanticEnd - semanticStart)};

				const wchar_t *sourceTagStart = findTag(L"source=", inputTag) + 9;
				if((size_t)sourceTagStart == 9)
					break;
				const wchar_t *sourceTagEnd = findTag(L"\"", sourceTagStart);
				if(!sourceTagEnd)
					break;
				std::wstring_view sourceValue = {sourceTagStart, (size_t)(sourceTagEnd - sourceTagStart)};

				const wchar_t *offsetTagStart = findTag(L"offset=", inputTag) + 8;
				if((size_t)offsetTagStart == 8)
					break;
				const wchar_t *offsetTagEnd = findTag(L"\"", offsetTagStart);
				if(!offsetTagEnd)
					break;
				std::wstring_view offsetTag = {offsetTagStart, (size_t)(offsetTagEnd - offsetTagStart)};
				int offsetValue = _wtoi(offsetTag.data());

				if(semanticValue == L"VERTEX")
				{
					tg.vertexSource = sourceValue;
					tg.vertexOffset = offsetValue;
				}
				if(semanticValue == L"NORMAL")
				{
					tg.normalSource = sourceValue;
					tg.normalOffset = offsetValue;
				}
				if(semanticValue == L"TEXCOORD")
				{
					tg.texcoordSource = sourceValue;
					tg.texcoordOffset = offsetValue;
				}
				triTag = offsetTagEnd;
				continue;
			}
			text = materialTagEnd;
			const wchar_t *pTagStart = findTag(L"<p", materialTagEnd);
			if(!pTagStart)
				break;
			const wchar_t *pTagEnd = findTag(L"</p", pTagStart);
			if(!pTagEnd)
				break;
			if(pTagStart > triClosingTag)
				break;
			const std::wstring_view pText = getElementText(pTagStart);
			if(pText.data() == nullptr)
				break;
			parseIntArray(pText, tg);
			collada.triangleGroups.push_back(tg);
		}
	}
}

ColladaData::~ColladaData()
{
	delete[] daeData;
}

void parseCollada(const char *filepath, ColladaData& collada)
{
	FILE *file = fopen(filepath, "r");
	if(!file)
	{
		puts("Failed to open file.");
		return;
	}
	collada.daeData = new wchar_t[MAX_FILE_SIZE];
	collada.daeDataEnd = collada.daeData;
	wchar_t *writePos = collada.daeData;
	wchar_t c = fgetwc(file);
	while(!feof(file))
	{
		if(writePos == collada.daeData + MAX_FILE_SIZE - 1) break;
		*writePos = c;
		collada.daeDataEnd++;
		writePos++;
		c = fgetwc(file);
	}
	*writePos = 0;
	fclose(file);

	const wchar_t *meshTag = findTag(L"<mesh", collada.daeData);
	if(!meshTag)
	{
		puts("Failed to find mesh tag.");
		return;
	}
	findSourceIds(meshTag, collada.daeDataEnd, collada);
}
