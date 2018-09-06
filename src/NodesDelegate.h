#pragma once

#include "Nodes.h"
#include "Evaluation.h"

struct TileNodeEditGraphDelegate : public NodeGraphDelegate
{
	struct ImogenNode
	{
		size_t mType;
		unsigned int mEvaluationTexture;
		void *mParams;
	};

	std::vector<ImogenNode> mNodes;

	enum ConTypes
	{
		Con_Float,
		Con_Float2,
		Con_Float3,
		Con_Float4,
		Con_Color4,
		Con_Int,
		Con_Ramp,
		Con_Angle,
		Con_Angle2,
		Con_Angle3,
		Con_Angle4,
		Con_Enum,
		Con_Structure,
		Con_Any,
	};
	virtual unsigned char *GetParamBlock(int index, size_t& paramBlockSize)
	{
		const ImogenNode & node = mNodes[index];
		paramBlockSize = ComputeParamMemSize(node.mType);
		return (unsigned char*)node.mParams;
	}
	virtual void SetParamBlock(int index, unsigned char* paramBlock)
	{
		const ImogenNode & node = mNodes[index];
		memcpy(node.mParams, paramBlock, ComputeParamMemSize(node.mType));
		SetEvaluationCall(node.mEvaluationTexture, ComputeFunctionCall(index));
	}

	virtual bool AuthorizeConnexion(int typeA, int typeB)
	{
		return true;
	}

	virtual unsigned int GetNodeTexture(size_t index)
	{
		return GetEvaluationTexture(mNodes[index].mEvaluationTexture);
	}
	virtual void AddNode(size_t type)
	{
		size_t index = mNodes.size();
		ImogenNode node;
		node.mEvaluationTexture = AddEvaluationTarget();
		node.mType = type;
		size_t paramsSize = ComputeParamMemSize(type);
		node.mParams = malloc(paramsSize);
		memset(node.mParams, 0, paramsSize);
		mNodes.push_back(node);

		SetEvaluationCall(node.mEvaluationTexture, ComputeFunctionCall(index));
	}

	void AddLink(int InputIdx, int InputSlot, int OutputIdx, int OutputSlot)
	{
		AddEvaluationInput(OutputIdx, OutputSlot, InputIdx);
	}

	virtual void DelLink(int index, int slot)
	{
		DelEvaluationInput(index, slot);
	}

	virtual void DeleteNode(size_t index)
	{
		DelEvaluationTarget(index);
		free(mNodes[index].mParams);
		mNodes.erase(mNodes.begin() + index);
		for (auto& node : mNodes)
		{
			if (node.mEvaluationTexture > index)
				node.mEvaluationTexture--;
		}
	}
	virtual const MetaNode* GetMetaNodes(int &metaNodeCount)
	{
		static const uint32_t hcTransform = IM_COL32(200, 200, 200, 255);
		static const uint32_t hcGenerator = IM_COL32(150, 200, 150, 255);
		static const uint32_t hcMaterial = IM_COL32(150, 150, 200, 255);
		static const uint32_t hcBlend = IM_COL32(200, 150, 150, 255);
		static const uint32_t hcFilter = IM_COL32(200, 200, 150, 255);

		metaNodeCount = 18;

		static const MetaNode metaNodes[18] = {
			{
				"Circle", hcGenerator
				,{ {} }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "Radius", (int)Con_Float, 0.f,1.f,0.f,0.f },{ "T", (int)Con_Float } }
			}
			,
			{
				"Transform", hcTransform
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "Translate", (int)Con_Float2, 1.f,0.f,1.f,0.f },{ "Rotation", (int)Con_Angle },{ "Scale", (int)Con_Float } }
			}
			,
			{
				"Square", hcGenerator
				,{ { } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "Width", (int)Con_Float } }
			}
			,
			{
				"Checker", hcGenerator
				,{ {} }
			,{ { "Out", (int)Con_Float4 } }
			,{  }
			}
			,
			{
				"Sine", hcGenerator
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "Frequency", (int)Con_Float },{ "Angle", (int)Con_Angle } }
			}

			,
			{
				"SmoothStep", hcFilter
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "Low", (int)Con_Float },{ "High", (int)Con_Float } }
			}

			,
			{
				"Pixelize", hcTransform
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "scale", (int)Con_Float } }
			}


			,
			{
				"Blur", hcFilter
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "angle", (int)Con_Angle },{ "strength", (int)Con_Float } }
			}

			,
			{
				"NormalMap", hcFilter
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "spread", (int)Con_Float } }
			}

			,
			{
				"LambertMaterial", hcMaterial
				,{ { "Diffuse", (int)Con_Float4 },{ "Normal", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "view", (int)Con_Float2, 1.f,0.f,0.f,1.f } }
			}

			,
			{
				"MADD", hcBlend
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "Mul Color", (int)Con_Color4 }, {"Add Color", (int)Con_Color4} }
			}
			
			,
			{
				"Hexagon", hcGenerator
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{  }
			}

			,
			{
				"Blend", hcBlend
				,{ { "A", (int)Con_Float4 },{ "B", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ {"A", (int)Con_Float4 },{ "B", (int)Con_Float4 },{ "Operation", (int)Con_Enum, 0.f,0.f,0.f,0.f, "Add\0Mul\0Min\0Max\0" } }
			}

			,
			{
				"Invert", hcFilter
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{}
			}

			,
			{
				"CircleSplatter", hcGenerator
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "Distance", (int)Con_Float2 },{ "Radius", (int)Con_Float2 },{ "Angle", (int)Con_Angle2 },{ "Count", (int)Con_Float } }
			}

			,
			{
				"Ramp", hcFilter
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "Ramp", (int)Con_Ramp } }
			}

			,
			{
				"Tile", hcTransform
				,{ { "In", (int)Con_Float4 } }
			,{ { "Out", (int)Con_Float4 } }
			,{ { "Scale", (int)Con_Float },{ "Offset 0", (int)Con_Float2 },{ "Offset 1", (int)Con_Float2 },{ "Overlap", (int)Con_Float2 } }
			}

				,
				{
					"Color", hcGenerator
					,{  }
				,{ { "Out", (int)Con_Float4 } }
				,{ { "Color", (int)Con_Color4 } }
				}
			};

		return metaNodes;
	}

	std::string ComputeFunctionCall(size_t index)
	{
		int metaNodeCount;
		const MetaNode* metaNodes = GetMetaNodes(metaNodeCount);
		const MetaNode &metaNode = metaNodes[mNodes[index].mType];
		std::string call(metaNode.mName);
		call += "(vUV";


		const NodeGraphDelegate::Con * param = metaNodes[mNodes[index].mType].mParams;
		unsigned char *paramBuffer = (unsigned char*)mNodes[index].mParams;
		char tmps[512];
		for (int i = 0; i < MaxCon; i++, param++)
		{
			if (!param->mName)
				break;
			switch (param->mType)
			{
			case Con_Angle:
			case Con_Float:
				sprintf(tmps, ",%f", *(float*)paramBuffer);
				break;
			case Con_Angle2:
			case Con_Float2:
				sprintf(tmps, ",vec2(%f, %f)", ((float*)paramBuffer)[0], ((float*)paramBuffer)[1]);
				break;
			case Con_Angle3:
			case Con_Float3:
				sprintf(tmps, ",vec3(%f, %f, %f)", ((float*)paramBuffer)[0], ((float*)paramBuffer)[1], ((float*)paramBuffer)[2]);
				break;
			case Con_Angle4:
			case Con_Color4:
			case Con_Float4:
				sprintf(tmps, ",vec4(%f, %f, %f, %f)", ((float*)paramBuffer)[0], ((float*)paramBuffer)[1], ((float*)paramBuffer)[2], ((float*)paramBuffer)[3]);
				break;
			case Con_Enum:
			case Con_Int:
				sprintf(tmps, ",%d", *(int*)paramBuffer);
				break;
			case Con_Ramp:
				sprintf(tmps, ",vec2[](vec2(%f,%f),vec2(%f,%f),vec2(%f,%f),vec2(%f,%f),vec2(%f,%f),vec2(%f,%f),vec2(%f,%f),vec2(%f,%f))"
					, ((float*)paramBuffer)[0], ((float*)paramBuffer)[1]
					, ((float*)paramBuffer)[2], ((float*)paramBuffer)[3]
					, ((float*)paramBuffer)[4], ((float*)paramBuffer)[5]
					, ((float*)paramBuffer)[6], ((float*)paramBuffer)[7]
					, ((float*)paramBuffer)[8], ((float*)paramBuffer)[9]
					, ((float*)paramBuffer)[10], ((float*)paramBuffer)[11]
					, ((float*)paramBuffer)[12], ((float*)paramBuffer)[13]
					, ((float*)paramBuffer)[14], ((float*)paramBuffer)[15]);
			break;
			}
			call += tmps;
			paramBuffer += ComputeParamMemSize(param->mType);
		}
		call += ")";
		return call;
	}

	const float PI = 3.14159f;
	float RadToDeg(float a) { return a * 180.f / PI; }
	float DegToRad(float a) { return a / 180.f * PI; }
	void EditNode()
	{
		size_t index = mSelectedNodeIndex;

		int metaNodeCount;
		const MetaNode* metaNodes = GetMetaNodes(metaNodeCount);
		bool dirty = false;
		const MetaNode& currentMeta = metaNodes[mNodes[index].mType];
		if (!ImGui::CollapsingHeader(currentMeta.mName, 0, ImGuiTreeNodeFlags_DefaultOpen))
			return;

		const NodeGraphDelegate::Con * param = currentMeta.mParams;
		unsigned char *paramBuffer = (unsigned char*)mNodes[index].mParams;
		for (int i = 0; i < MaxCon; i++, param++)
		{
			if (!param->mName)
				break;
			switch (param->mType)
			{
			case Con_Float:
				dirty |= ImGui::InputFloat(param->mName, (float*)paramBuffer);
				break;
			case Con_Float2:
				dirty |= ImGui::InputFloat2(param->mName, (float*)paramBuffer);
				break;
			case Con_Float3:
				dirty |= ImGui::InputFloat3(param->mName, (float*)paramBuffer);
				break;
			case Con_Float4:
				dirty |= ImGui::InputFloat4(param->mName, (float*)paramBuffer);
				break;
			case Con_Color4:
				dirty |= ImGui::ColorPicker4(param->mName, (float*)paramBuffer);
				break;
			case Con_Int:
				dirty |= ImGui::InputInt(param->mName, (int*)paramBuffer);
				break;
			case Con_Ramp:
				{
					char tmps[512];
					for (int k = 0; k < 8; k++)
					{
						sprintf(tmps, "Ramp %d", k);
						dirty |= ImGui::InputFloat2(tmps, &((float*)paramBuffer)[k * 2]);
					}
				
				}
				break;
			case Con_Angle:
				((float*)paramBuffer)[0] = RadToDeg(((float*)paramBuffer)[0]);
				dirty |= ImGui::InputFloat(param->mName, (float*)paramBuffer);
				((float*)paramBuffer)[0] = DegToRad(((float*)paramBuffer)[0]);
				break;
			case Con_Angle2:
				((float*)paramBuffer)[0] = RadToDeg(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = RadToDeg(((float*)paramBuffer)[1]);
				dirty |= ImGui::InputFloat2(param->mName, (float*)paramBuffer);
				((float*)paramBuffer)[0] = DegToRad(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = DegToRad(((float*)paramBuffer)[1]);
				break;
			case Con_Angle3:
				((float*)paramBuffer)[0] = RadToDeg(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = RadToDeg(((float*)paramBuffer)[1]);
				((float*)paramBuffer)[2] = RadToDeg(((float*)paramBuffer)[2]);
				dirty |= ImGui::InputFloat3(param->mName, (float*)paramBuffer);
				((float*)paramBuffer)[0] = DegToRad(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = DegToRad(((float*)paramBuffer)[1]);
				((float*)paramBuffer)[2] = DegToRad(((float*)paramBuffer)[2]);
				break;
			case Con_Angle4:
				((float*)paramBuffer)[0] = RadToDeg(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = RadToDeg(((float*)paramBuffer)[1]);
				((float*)paramBuffer)[2] = RadToDeg(((float*)paramBuffer)[2]);
				((float*)paramBuffer)[3] = RadToDeg(((float*)paramBuffer)[3]);
				dirty |= ImGui::InputFloat4(param->mName, (float*)paramBuffer);
				((float*)paramBuffer)[0] = DegToRad(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = DegToRad(((float*)paramBuffer)[1]);
				((float*)paramBuffer)[2] = DegToRad(((float*)paramBuffer)[2]);
				((float*)paramBuffer)[3] = DegToRad(((float*)paramBuffer)[3]);
				break;
			case Con_Enum:
				dirty |= ImGui::Combo(param->mName, (int*)paramBuffer, param->mEnumList);
				break;
			}
			paramBuffer += ComputeParamMemSize(param->mType);
		}
		
		//ImGui::End();
		if (dirty)
			SetEvaluationCall(mNodes[index].mEvaluationTexture, ComputeFunctionCall(index));
	}

	// helpers
	/*
	size_t ComputeParamsMemSize()
	{
		int metaNodeCount;
		const MetaNode* metaNodes = GetMetaNodes(metaNodeCount);
		size_t res = 0;
		for (size_t typeIndex = 0; typeIndex < metaNodeCount; typeIndex++)
		{
			res += ComputeParamMemSize(typeIndex);
		}
		return res;
	}
	*/
	void UpdateAllFunctionCalls()
	{
		for(size_t i = 0;i<mNodes.size();i++)
			SetEvaluationCall(mNodes[i].mEvaluationTexture, ComputeFunctionCall(i));
	}

	void SetMouseRatios(float rx, float ry)
	{
		int metaNodeCount;
		const MetaNode* metaNodes = GetMetaNodes(metaNodeCount);
		size_t res = 0;
		const NodeGraphDelegate::Con * param = metaNodes[mNodes[mSelectedNodeIndex].mType].mParams;
		unsigned char *paramBuffer = (unsigned char*)mNodes[mSelectedNodeIndex].mParams;
		for (int i = 0; i < MaxCon; i++, param++)
		{
			if (!param->mName)
				break;
			float *paramFlt = (float*)paramBuffer;
			if (param->mRangeMinX != 0.f || param->mRangeMaxX != 0.f)
			{
				paramFlt[0] = ImLerp(param->mRangeMinX, param->mRangeMaxX, rx);
			}
			if (param->mRangeMinY != 0.f || param->mRangeMaxY != 0.f)
			{
				paramFlt[1] = ImLerp(param->mRangeMinY, param->mRangeMaxY, ry);
			}
			paramBuffer += ComputeParamMemSize(param->mType);
		}
		SetEvaluationCall(mNodes[mSelectedNodeIndex].mEvaluationTexture, ComputeFunctionCall(mSelectedNodeIndex));
	}

	size_t ComputeParamMemSize(size_t typeIndex)
	{
		int metaNodeCount;
		const MetaNode* metaNodes = GetMetaNodes(metaNodeCount);
		size_t res = 0;
		const NodeGraphDelegate::Con * param = metaNodes[typeIndex].mParams;
		for (int i = 0; i < MaxCon; i++, param++)
		{
			if (!param->mName)
				break;
			res += ComputeParamMemSize(param->mType);
		}
		return res;
	}
	size_t ComputeParamMemSize(int paramType)
	{
		size_t res = 0;
		switch (paramType)
		{
		case Con_Angle:
		case Con_Float:
			res += sizeof(float);
			break;
		case Con_Angle2:
		case Con_Float2:
			res += sizeof(float) * 2;
			break;
		case Con_Angle3:
		case Con_Float3:
			res += sizeof(float) * 3;
			break;
		case Con_Angle4:
		case Con_Color4:
		case Con_Float4:
			res += sizeof(float) * 4;
			break;
		case Con_Ramp:
			res += sizeof(float) * 2 * 8;
			break;
		case Con_Enum:
		case Con_Int:
			res += sizeof(int);
			break;
			
		}
		return res;
	}

	virtual void UpdateEvaluationList(const std::vector<int> nodeOrderList)
	{
		SetEvaluationOrder(nodeOrderList);
	}
};

