#include "../include/essl.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef USE_VITASDK
#include <psp2/shacccg.h>
#include "../include/shacccg/paramquery.h"
#else
#include <shacccg.h>
#endif

typedef struct EsslParameterListNode EsslParameterListNode;

typedef struct EsslParameterListNode
{
    EsslParameterListNode *next;
    union 
    {
        EsslParameter parameter;
        EsslVaryingParameter varyingParameter;
    };
    size_t nameLength;
} EsslParameterListNode;

typedef struct EsslParameterList
{
    EsslParameterListNode *head;
    size_t count;
    size_t totalNameSize;
} EsslParameterList;

inline size_t EsslParameterSetName(EsslParameter *parameter, SceShaccCgParameter shaccParameter, const char *parentName)
{
    const char *paramName = sceShaccCgGetParameterName(shaccParameter);

    // Only the vertex program will encounter this, so make it nice.
    if ((strcmp(paramName, "main") == 0) && parentName == NULL)
        paramName = "gl_Position";

    size_t nameLength = strlen(paramName) + 1;

    if (parentName != NULL)
        nameLength += strlen(parentName);

    parameter->parameterName = calloc(nameLength, sizeof(char));
    if (parameter->parameterName == NULL)
        return 0;

    if (parentName)
        strcat(parameter->parameterName, parentName);

    strcat(parameter->parameterName, paramName);

    return nameLength;
}

size_t EsslParameterCreate(EsslParameter *parameter, SceShaccCgParameter shaccParameter, EsslParameterType type, size_t elementCount, const char *parentName)
{
    size_t columnCount = sceShaccCgGetParameterColumns(shaccParameter);
    size_t rowCount = sceShaccCgGetParameterRows(shaccParameter);
    size_t vectorWidth = sceShaccCgGetParameterVectorWidth(shaccParameter);

    SceShaccCgParameterClass parameterClass = sceShaccCgGetParameterClass(shaccParameter);
    SceShaccCgParameterBaseType baseType = sceShaccCgGetParameterBaseType(shaccParameter);
    switch (parameterClass)
    {
    case SCE_SHACCCG_PARAMETERCLASS_SCALAR:
        if (type == ESSL_PARAMETER_TYPE_ATTRIBUTE)
        {
            switch (baseType)
            {
            case SCE_SHACCCG_BASETYPE_BOOL:
            case SCE_SHACCCG_BASETYPE_CHAR:
            case SCE_SHACCCG_BASETYPE_SHORT:
            case SCE_SHACCCG_BASETYPE_INT:
            case SCE_SHACCCG_BASETYPE_FIXED:
            case SCE_SHACCCG_BASETYPE_HALF:
            case SCE_SHACCCG_BASETYPE_FLOAT:
                parameter->format = ESSL_PARAMETER_FORMAT_FLOAT;
                break;
            default:
                return 0;
            }
        }
        else
        {
            switch (baseType)
            {
            case SCE_SHACCCG_BASETYPE_BOOL:
                parameter->format = ESSL_PARAMETER_FORMAT_BOOL;
                break;
            case SCE_SHACCCG_BASETYPE_CHAR:
            case SCE_SHACCCG_BASETYPE_SHORT:
            case SCE_SHACCCG_BASETYPE_INT:
                parameter->format = ESSL_PARAMETER_FORMAT_INT;
                break;
            case SCE_SHACCCG_BASETYPE_FIXED:
            case SCE_SHACCCG_BASETYPE_HALF:
            case SCE_SHACCCG_BASETYPE_FLOAT:
                parameter->format = ESSL_PARAMETER_FORMAT_FLOAT;
                break;
            default:
                return 0;
            }
        }    
        break;
    case SCE_SHACCCG_PARAMETERCLASS_VECTOR:
        if (type == ESSL_PARAMETER_TYPE_ATTRIBUTE)
        {
            switch (baseType)
            {
            case SCE_SHACCCG_BASETYPE_BOOL:
            case SCE_SHACCCG_BASETYPE_CHAR:
            case SCE_SHACCCG_BASETYPE_SHORT:
            case SCE_SHACCCG_BASETYPE_INT:
            case SCE_SHACCCG_BASETYPE_FIXED:
            case SCE_SHACCCG_BASETYPE_HALF:
            case SCE_SHACCCG_BASETYPE_FLOAT:
                parameter->format = ESSL_PARAMETER_FORMAT_FLOAT + (vectorWidth - 1);
                break;
            default:
                return 0;
            }
        }
        else
        {
            switch (baseType)
            {
            case SCE_SHACCCG_BASETYPE_BOOL:
                parameter->format = ESSL_PARAMETER_FORMAT_BOOL + (vectorWidth - 1);
                break;
            case SCE_SHACCCG_BASETYPE_CHAR:
            case SCE_SHACCCG_BASETYPE_SHORT:
            case SCE_SHACCCG_BASETYPE_INT:
                parameter->format = ESSL_PARAMETER_FORMAT_INT + (vectorWidth - 1);
                break;
            case SCE_SHACCCG_BASETYPE_FIXED:
            case SCE_SHACCCG_BASETYPE_HALF:
            case SCE_SHACCCG_BASETYPE_FLOAT:
                parameter->format = ESSL_PARAMETER_FORMAT_FLOAT + (vectorWidth - 1);
                break;
            default:
                return 0;
            }
        }
        break;
    case SCE_SHACCCG_PARAMETERCLASS_MATRIX:
        if (columnCount == 4 && rowCount == 4)
            parameter->format = ESSL_PARAMETER_FORMAT_MAT4;
        else if (columnCount == 3 && rowCount == 3)
            parameter->format = ESSL_PARAMETER_FORMAT_MAT3;
        else if (columnCount == 2 && rowCount == 2)
            parameter->format = ESSL_PARAMETER_FORMAT_MAT2;
        else
            return 0;
        break;
    case SCE_SHACCCG_PARAMETERCLASS_SAMPLER:
        switch (baseType)
        {
        case SCE_SHACCCG_BASETYPE_SAMPLER1D:
        case SCE_SHACCCG_BASETYPE_ISAMPLER1D:
        case SCE_SHACCCG_BASETYPE_USAMPLER1D:
        case SCE_SHACCCG_BASETYPE_SAMPLER2D:
        case SCE_SHACCCG_BASETYPE_ISAMPLER2D:
        case SCE_SHACCCG_BASETYPE_USAMPLER2D:
            parameter->format = ESSL_PARAMETER_FORMAT_SAMPLER2D;
            break;
        case SCE_SHACCCG_BASETYPE_SAMPLERCUBE:
        case SCE_SHACCCG_BASETYPE_ISAMPLERCUBE:
        case SCE_SHACCCG_BASETYPE_USAMPLERCUBE:
            parameter->format = ESSL_PARAMETER_FORMAT_SAMPLERCUBE;
            break;
        default:
            // Unreachable.
            break;
        }
        break;
    default:
        // Unreachable.
        break;
    }

    parameter->type = type;
    parameter->elementCount = elementCount;
    parameter->state = sceShaccCgIsParameterReferenced(shaccParameter);

    if (parameter->type >= ESSL_PARAMETER_TYPE_VARYING_INPUT)
    {
        const char *semantic = sceShaccCgGetParameterSemantic(shaccParameter);
        size_t resourceIndex = sceShaccCgGetParameterResourceIndex(shaccParameter);

        EsslVaryingParameter *varyingParameter = (EsslVaryingParameter *)parameter;

        if (semantic != NULL)
        {
            if ((strncmp(semantic, "TEXCOORD", strlen("TEXCOORD")) == 0) || (strncmp(semantic, "TEX", strlen("TEX")) == 0))
            {
                varyingParameter->varyingType = ESSL_VARYING_TYPE_TEXCOORD;
                varyingParameter->resourceIndex = resourceIndex;
            }
            else if (strncmp(semantic, "POSITION", strlen("POSITION")) == 0)
            {
                varyingParameter->varyingType = ESSL_VARYING_TYPE_POSITION;
                varyingParameter->resourceIndex = 0;
            }
            else if ((strncmp(semantic, "PSIZE", strlen("PSIZE")) == 0) || (strncmp(semantic, "PSIZ", strlen("PSIZ")) == 0))
            {
                varyingParameter->varyingType = ESSL_VARYING_TYPE_POINTSIZE;
                varyingParameter->resourceIndex = 0;
            }
            else if (strncmp(semantic, "FACE", strlen("FACE")) == 0)
            {
                varyingParameter->varyingType = ESSL_VARYING_TYPE_FRONTFACE;
                varyingParameter->resourceIndex = 0;
            }
            else if (strncmp(semantic, "WPOS", strlen("WPOS")) == 0)
            {
                varyingParameter->varyingType = ESSL_VARYING_TYPE_FRAGCOORD;
                varyingParameter->resourceIndex = 0;
            }
            else if ((strncmp(semantic, "SPRITECOORD", strlen("SPRITECOORD")) == 0) || (strncmp(semantic, "POINTCOORD", strlen("POINTCOORD")) == 0))
            {
                varyingParameter->varyingType = ESSL_VARYING_TYPE_POINTCOORD;
                varyingParameter->resourceIndex = 0;
            }
            else
            {
                varyingParameter->varyingType = -1;
                varyingParameter->resourceIndex = -1;
            }
        }
        else
        {
            varyingParameter->varyingType = -1;
            varyingParameter->resourceIndex = -1;
        }

        memset(varyingParameter->padding, 0xFF, 0x24);
    }

    if (elementCount == 1)
        return EsslParameterSetName(parameter, shaccParameter, parentName);
    else
        return 1;
}

inline void EsslParameterListLink(EsslParameterList *parameterList, EsslParameterListNode *node)
{
    node->next = parameterList->head;
    parameterList->head = node;
    parameterList->count++;
    parameterList->totalNameSize += node->nameLength;
}

int EsslParameterListCreateArray(EsslParameterList *parameterList, SceShaccCgParameter *parameter, EsslParameterType paramType, const char *parentName);
int EsslParameterListCreateStructure(EsslParameterList *parameterList, SceShaccCgParameter *parameter, EsslParameterType paramType, const char *parentName, int arrayParent)
{
    EsslParameterListNode *currentNode = NULL;
    SceShaccCgParameter currentParameter = sceShaccCgGetFirstStructParameter(parameter);
    SceShaccCgParameterClass parameterClass;

    char *paramName;
    const char *structName = sceShaccCgGetParameterName(parameter);

    if (arrayParent != 0)
    {
        paramName = malloc((strlen(parentName) + strlen(structName) + 4) * sizeof(char));
        if (paramName == NULL)
            return 0;
        
        sprintf(paramName, "%s[%s].", parentName, structName);
    }
    else
    {
        if (parentName != NULL)
        {
            paramName = malloc((strlen(parentName) + strlen(structName) + 2) * sizeof(char));
            if (paramName == NULL)
                return 0;

            sprintf(paramName, "%s%s.", parentName, structName);
        }
        else
        {
            paramName = malloc((strlen(structName) + 2) * sizeof(char));
            if (paramName == NULL)
                return 0;

            sprintf(paramName, "%s.", structName);
        }
    }

    while (currentParameter != NULL)
    {
        parameterClass = sceShaccCgGetParameterClass(currentParameter);
        switch (parameterClass)
        {
        case SCE_SHACCCG_PARAMETERCLASS_STRUCT:
            if (EsslParameterListCreateStructure(parameterList, currentParameter, paramType, paramName, 0) == 0)
                goto fail;
            break;
        case SCE_SHACCCG_PARAMETERCLASS_ARRAY:
            if (EsslParameterListCreateArray(parameterList, currentParameter, paramType, paramName) == 0)
                goto fail;
            break;
        case SCE_SHACCCG_PARAMETERCLASS_SCALAR:
        case SCE_SHACCCG_PARAMETERCLASS_VECTOR:
        case SCE_SHACCCG_PARAMETERCLASS_MATRIX:
        case SCE_SHACCCG_PARAMETERCLASS_SAMPLER:
            currentNode = malloc(sizeof(EsslParameterListNode));
            if (currentNode == NULL)
                goto fail;

            currentNode->nameLength = EsslParameterCreate(&currentNode->parameter, currentParameter, paramType, 1, paramName);
            if (currentNode->nameLength == 0)
                goto fail;

            EsslParameterListLink(parameterList, currentNode);
            break;
        default:
            break;
        }
        
        currentParameter = sceShaccCgGetNextParameter(currentParameter);
        continue;

    fail:
        if (currentNode != NULL)
            free(currentNode);

        free(paramName);
        return 0;
    }

    free(paramName);
    return 1;
}

int EsslParameterListCreateArray(EsslParameterList *parameterList, SceShaccCgParameter *parameter, EsslParameterType paramType, const char *parentName)
{
    const char *arrayName = sceShaccCgGetParameterName(parameter);
    char *paramName = NULL;

    EsslParameterListNode *currentNode = NULL;
    SceShaccCgParameter currentParameter = sceShaccCgGetArrayParameter(parameter, 0);
    SceShaccCgParameterClass parameterClass;

    parameterClass = sceShaccCgGetParameterClass(currentParameter);
    switch (parameterClass)
    {
    case SCE_SHACCCG_PARAMETERCLASS_STRUCT:
        if (parentName != NULL)
        {
            paramName = malloc((strlen(parentName) + strlen(arrayName) + 1) * sizeof(char));
            if (paramName == NULL)
                goto fail;
            
            sprintf(paramName, "%s%s", parentName, arrayName);
        }
        else
            paramName = arrayName;

        while (currentParameter != NULL)
        {
            if (EsslParameterListCreateStructure(parameterList, currentParameter, paramType, paramName, 1) == 0)
                goto fail;

            currentParameter = sceShaccCgGetNextParameter(currentParameter);
        }

        if (paramName != arrayName)
            free(paramName);
        break;
    case SCE_SHACCCG_PARAMETERCLASS_SCALAR:
    case SCE_SHACCCG_PARAMETERCLASS_VECTOR:
    case SCE_SHACCCG_PARAMETERCLASS_MATRIX:
    case SCE_SHACCCG_PARAMETERCLASS_SAMPLER:
        currentNode = malloc(sizeof(EsslParameterListNode));
        if (currentNode == NULL)
            goto fail;

        if (EsslParameterCreate(&currentNode->parameter, currentParameter, paramType, sceShaccCgGetArraySize(parameter), NULL) == 0)
            goto fail;

        currentNode->nameLength = EsslParameterSetName(&currentNode->parameter, parameter, parentName);
        if (currentNode->nameLength == 0)
            goto fail;

        EsslParameterListLink(parameterList, currentNode);
        break;
    default:
        goto fail;
    }

    return 1;
fail:
    if (currentNode != NULL)
        free(currentNode);

    return 0;
}

int EsslParameterListCreate(EsslParameterList *parameterList, SceShaccCgCompileOutput *compileOutput, EsslParameterType paramType)
{
    SceShaccCgParameterVariability targetVariabiltiy = SCE_SHACCCG_VARIABILITY_INVALID;
    SceShaccCgParameterDirection targetDirection = SCE_SHACCCG_DIRECTION_INVALID;

    switch (paramType)
    {
    case ESSL_PARAMETER_TYPE_ATTRIBUTE:
        targetVariabiltiy = SCE_SHACCCG_VARIABILITY_VARYING;
        targetDirection = SCE_SHACCCG_DIRECTION_IN;
        break;
    case ESSL_PARAMETER_TYPE_UNIFORM:
        targetVariabiltiy = SCE_SHACCCG_VARIABILITY_UNIFORM;
        targetDirection = SCE_SHACCCG_DIRECTION_IN;
        break;
    case ESSL_PARAMETER_TYPE_VARYING_INPUT:
        targetVariabiltiy = SCE_SHACCCG_VARIABILITY_VARYING;
        targetDirection = SCE_SHACCCG_DIRECTION_IN;
        break;
    case ESSL_PARAMETER_TYPE_VARYING_OUTPUT:
        targetVariabiltiy = SCE_SHACCCG_VARIABILITY_VARYING;
        targetDirection = SCE_SHACCCG_DIRECTION_OUT;
        break;
    }

    EsslParameterListNode *currentNode = NULL;
    SceShaccCgParameter currentParameter = sceShaccCgGetFirstParameter(compileOutput);
    SceShaccCgParameterClass parameterClass = SCE_SHACCCG_PARAMETERCLASS_INVALID;

    while (currentParameter != NULL)
    {
        if ((sceShaccCgGetParameterDirection(currentParameter) != targetDirection) || (sceShaccCgGetParameterVariability(currentParameter) != targetVariabiltiy))
            goto next;

        parameterClass = sceShaccCgGetParameterClass(currentParameter);
        switch (parameterClass)
        {
        case SCE_SHACCCG_PARAMETERCLASS_STRUCT:
            if (EsslParameterListCreateStructure(parameterList, currentParameter, paramType, NULL, 0) == 0)
                goto fail;
            break;
        case SCE_SHACCCG_PARAMETERCLASS_ARRAY:
            if (EsslParameterListCreateArray(parameterList, currentParameter, paramType, NULL) == 0)
                goto fail;
            break;
        case SCE_SHACCCG_PARAMETERCLASS_SCALAR:
        case SCE_SHACCCG_PARAMETERCLASS_VECTOR:
        case SCE_SHACCCG_PARAMETERCLASS_MATRIX:
        case SCE_SHACCCG_PARAMETERCLASS_SAMPLER:
            currentNode = malloc(sizeof(EsslParameterListNode));
            if (currentNode == NULL)
                goto fail;

            currentNode->nameLength = EsslParameterCreate(&currentNode->parameter, currentParameter, paramType, 1, NULL);
            if (currentNode->nameLength == 0)
                goto fail;

            EsslParameterListLink(parameterList, currentNode);
            break;
        default:
            break;
        }

    next:
        currentParameter = sceShaccCgGetNextParameter(currentParameter);
        continue;

    fail:
        if (currentNode != NULL)
            free(currentNode);

        return 0;
    }

    return 1;
}

void* EsslParameterTableCreate(EsslParameterList *parameterLists, size_t *parameterTableSize)
{
    uint8_t *parameterTable;
    *parameterTableSize = sizeof(uint32_t) * 3;
    uint8_t *curPtr;

    char *namePtr;
    uint32_t nameOffset;

    *parameterTableSize += sizeof(EsslParameter) * parameterLists[0].count;
    *parameterTableSize += sizeof(EsslParameter) * parameterLists[1].count;
    *parameterTableSize += sizeof(EsslVaryingParameter) * parameterLists[2].count;
    nameOffset = *parameterTableSize;

    *parameterTableSize += parameterLists[0].totalNameSize;
    *parameterTableSize += parameterLists[1].totalNameSize;
    *parameterTableSize += parameterLists[2].totalNameSize;

    if (*parameterTableSize == sizeof(uint32_t) * 3)
        return NULL;

    parameterTable = malloc(*parameterTableSize);
    if (parameterTable == NULL)
        return NULL;

    curPtr = parameterTable;
    namePtr = parameterTable + nameOffset;

    for (int i = 2; i >= 0; i--)
    {
        *(uint32_t *)curPtr = parameterLists[i].count;
        curPtr += 4;

        EsslParameterListNode *node = parameterLists[i].head;
        while (node != NULL)
        {
            size_t parameterSize = node->parameter.type > 1 ? sizeof(EsslVaryingParameter) : sizeof(EsslParameter);
            memcpy(curPtr, &node->varyingParameter.header, parameterSize);

            char *paramName = node->parameter.parameterName;
            size_t nameLength = strlen(paramName) + 1;
            strcpy(namePtr, paramName);

            ((EsslParameter *)curPtr)->parameterNameOffset = (uint32_t)namePtr - (uint32_t)curPtr;

            namePtr += nameLength;
            curPtr += parameterSize;

            EsslParameterListNode *prevNode = node;
            node = node->next;

            free(prevNode->parameter.parameterName);
            free(prevNode);
        }
    }

    return parameterTable;
}

void EsslCreateBinary(SceShaccCgCompileOutput *compileOutput, void **binary, size_t *binarySize, int vertexShader)
{
    EsslHeader header = {"GXPES", 0, 0};
    EsslParameterList parameterLists[3] = {0};
    void *parameterTable = NULL;
    size_t parameterTableSize = 0;

    for (int i = 2; i >= 0; i--)
    {
        if (!vertexShader && i == 0)
            break;

        EsslParameterType type = i;
        
        if (vertexShader && type == ESSL_PARAMETER_TYPE_VARYING_INPUT)
            type = ESSL_PARAMETER_TYPE_VARYING_OUTPUT;
        
        // Don't attempt to process attributes for fragment shader.
        if (vertexShader == 0 && i == 0)
            break;

        if (EsslParameterListCreate(&parameterLists[i], compileOutput, type) == 0)
            goto error;
    }

    parameterTable = EsslParameterTableCreate(parameterLists, &parameterTableSize);
    if (parameterTable == NULL)
        goto error;

    *binarySize = (compileOutput->programSize + 0xB) + parameterTableSize;
    *binary = malloc(*binarySize);
    if (*binary == NULL)
        goto error;

    memcpy(*binary + 0xB, compileOutput->programData, compileOutput->programSize);

    header.parameterTableSize = parameterTableSize;
    header.parameterTableOffset = *binarySize - parameterTableSize;

    memcpy(*binary, &header, sizeof(EsslHeader));
    memcpy(*binary + header.parameterTableOffset, parameterTable, parameterTableSize);

    return;
error:
    *binary = malloc(compileOutput->programSize);
    memcpy(*binary, compileOutput->programData, compileOutput->programSize);
    *binarySize = compileOutput->programSize;

    for (int i = 0; i < 2; i++)
    {
        EsslParameterListNode *node = parameterLists[i].head;
        while (node != NULL)
        {
            if (node->parameter.parameterName)
                free(node->parameter.parameterName);

            EsslParameterListNode *prevNode = node;
            node = node->next;

            free(prevNode);
        }
    }

    if (parameterTable != NULL)
        free(parameterTable);

    return;
}