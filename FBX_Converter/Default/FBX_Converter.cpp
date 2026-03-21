#include <filesystem>
#include <iostream>
#include <fstream>
#include <stack>
#include <Windows.h>
#include <string>
#include <list>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else //Release
#pragma comment(lib, "assimp-vc143-mt.lib")
#endif

struct Uint4
{
	unsigned int x{};
	unsigned int y{};
	unsigned int z{};
	unsigned int w{};
};

struct Float4
{
	float x{};
	float y{};
	float z{};
	float w{};
};

std::ofstream LogFile("../log.txt", std::ios::trunc);

void Log(const aiString& Text);
void Log(const char* Text);
void Write_AnimMeshes(std::ofstream& OutputFile, const aiScene* pAIScene, const std::vector<aiString>& BoneNames, int VTFMode);
void Write_StatMeshes(std::ofstream& OutputFile, const aiScene* pAIScene);
void Write_Materials(std::ofstream& OutputFile, const aiScene* pAIScene);

int main()
{
	unsigned int iNumStaticFiles{};
	unsigned int iNumAnimFiles{};
	Assimp::Importer Importer{};
	const aiScene* pAIScene{ nullptr };
	int iNumMesh{};
	std::string InputFilePath{};

	int iConvertMode{};

	int iForVTFMode{};

	Log("1. Static Only   2. Anim Only   3. All");
	std::cin >> iConvertMode;
	LogFile << iConvertMode << std::endl;

	if (iConvertMode != 2)
	{
		InputFilePath = "../StaticMesh/";
		Log("Static Models : Start Converting...");

	#pragma region Static Meshes
		for (const auto& entry : std::filesystem::recursive_directory_iterator(InputFilePath))
		{
			if (entry.is_regular_file())
			{
				if (entry.path().extension() != ".fbx")
				{
					continue;
				}

				Log(aiString("Converting... " + entry.path().filename().string()));

				std::filesystem::path relative_path = std::filesystem::relative(entry.path().parent_path(), InputFilePath);
				std::filesystem::path OutputFilePath = std::filesystem::path("../Output/Static/") / relative_path;
				if (!std::filesystem::exists(OutputFilePath))
				{
					std::filesystem::create_directories(OutputFilePath);
				}
				OutputFilePath /= entry.path().stem().string() + ".hyuntrastatmesh";
				std::ofstream OutputFile(OutputFilePath.c_str(), std::ios::binary);

				if (OutputFile.is_open())
				{
					pAIScene = Importer.ReadFile(entry.path().string(), aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
					if (pAIScene)
					{
						Log("Read File Succssefully");
					}

					Write_StatMeshes(OutputFile, pAIScene);
					Write_Materials(OutputFile, pAIScene);

					iNumStaticFiles++;
					OutputFile.close();
				}
			}
		}
	#pragma endregion

		Log("Static Models : Convert Success!\n");
	}

	if (iConvertMode != 1)
	{
		Log("Animation Models : Start Converting...");

		Log("1. Normally Anim Model	2. For VTF Model 3. Part Model");
		std::cin >> iForVTFMode;
		LogFile << iForVTFMode << std::endl;


	#pragma region Animation Meshes
		InputFilePath = "../AnimMesh/";
		for (const auto& entry : std::filesystem::recursive_directory_iterator(InputFilePath))
		{
			if (entry.is_regular_file())
			{
				if (entry.path().extension() != ".fbx")
				{
					continue;
				}
				Log(aiString("Converting... " + entry.path().filename().string()));

				// InputFilePath를 기준으로한 상대경로를 구한다.
				std::filesystem::path relative_path = std::filesystem::relative(entry.path().parent_path(), InputFilePath);

				// 연산자(/)로 Output경로와 위에서 구한 경로를 합친다.
				std::filesystem::path OutputFilePath = std::filesystem::path("../Output/Anim/") / relative_path;

				// 폴더가 없을때 생성한다.
				if (!std::filesystem::exists(OutputFilePath))
				{
					std::filesystem::create_directories(OutputFilePath);
				}

				// 최종 파일 경로를 생성
				if (3 != iForVTFMode)
					OutputFilePath /= entry.path().stem().string() + ".hyuntraanimmesh";
				else
					OutputFilePath /= entry.path().stem().string() + ".PartModel"; // 파츠 모델로 만들어봅시다

				std::ofstream OutputFile(OutputFilePath.c_str(), std::ios::binary);

				if (OutputFile.is_open())
				{
					pAIScene = Importer.ReadFile(entry.path().string(), aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
					if (pAIScene)
					{
						Log("Read File Succssefully");
					}
					std::vector<aiString> BoneNames{};
				#pragma region Bones
					std::stack<aiNode*> Nodes{};
					Nodes.push(pAIScene->mRootNode);
					unsigned int iNumBones{};
					while (!Nodes.empty())
					{
						aiNode* pCurrNode = Nodes.top();
						Nodes.pop();
						iNumBones++;

						for (size_t i = 0; i < pCurrNode->mNumChildren; i++)
						{
							Nodes.push(pCurrNode->mChildren[i]);
						}
					}
					if(3 != iForVTFMode)
						OutputFile.write(reinterpret_cast<const char*>(&iNumBones), sizeof(unsigned int));

					std::list<std::pair<aiNode*, int>> Bones{};
					int iMyIndex{ -1 };
					Bones.push_front({ pAIScene->mRootNode, -1 });

					while (!Bones.empty())
					{
						aiNode* pCurrentBone = Bones.front().first;
						int iParentIndex = Bones.front().second;
						Bones.pop_front();

						// 필요한 뼈 정보를 출력.
						char szName[_MAX_PATH]{};
						aiString BoneName = pCurrentBone->mName;
						BoneNames.push_back(BoneName);

						if (3 != iForVTFMode) {
							strcpy_s(szName, BoneName.C_Str());
							unsigned int iNameSize = static_cast<unsigned int>(strlen(szName)) + 1;
							OutputFile.write(reinterpret_cast<const char*>(&iNameSize), sizeof(unsigned int));
							OutputFile.write(BoneName.C_Str(), iNameSize);
							OutputFile.write(reinterpret_cast<const char*>(&pCurrentBone->mTransformation.Transpose()), sizeof(aiMatrix4x4));
							OutputFile.write(reinterpret_cast<const char*>(&iParentIndex), sizeof(int));
						}
						iMyIndex++;

						for (size_t i = 0; i < pCurrentBone->mNumChildren; i++)
						{
							Bones.push_back({ pCurrentBone->mChildren[i], iMyIndex });
						}
					}
				#pragma endregion

					Write_AnimMeshes(OutputFile, pAIScene, BoneNames, iForVTFMode);
					Write_Materials(OutputFile, pAIScene);

				#pragma region Animations
					if (3 != iForVTFMode) {
						unsigned int iNumAnimations = pAIScene->mNumAnimations;
						OutputFile.write(reinterpret_cast<const char*>(&iNumAnimations), sizeof(unsigned int));
						Log("Animation name list: ");
						for (size_t i = 0; i < iNumAnimations; i++)
						{
							aiAnimation* pAnimation = pAIScene->mAnimations[i];

							unsigned int iNameSize = pAnimation->mName.length + 1;
							OutputFile.write(reinterpret_cast<const char*>(&iNameSize), sizeof(unsigned int));
							OutputFile.write(pAnimation->mName.C_Str(), iNameSize);
							Log(pAnimation->mName.C_Str());

							float fDuration{ static_cast<float>(pAnimation->mDuration) };
							float fTickPerSec{ static_cast<float>(pAnimation->mTicksPerSecond) };
							OutputFile.write(reinterpret_cast<const char*>(&fDuration), sizeof(float));
							OutputFile.write(reinterpret_cast<const char*>(&fTickPerSec), sizeof(float));

							unsigned int iNumChannels = pAnimation->mNumChannels;
							OutputFile.write(reinterpret_cast<const char*>(&iNumChannels), sizeof(unsigned int));

							for (size_t j = 0; j < iNumChannels; j++)
							{
								aiNodeAnim* pChannel = pAnimation->mChannels[j];

								unsigned int iChannelNameSize = pChannel->mNodeName.length + 1;
								OutputFile.write(reinterpret_cast<const char*>(&iChannelNameSize), sizeof(unsigned int));
								OutputFile.write(pChannel->mNodeName.C_Str(), iChannelNameSize);

								unsigned int iBoneIndex{};
								auto iter = std::find_if(BoneNames.begin(), BoneNames.end(), [&pChannel, &iBoneIndex](aiString strBoneName)
									{
										if (pChannel->mNodeName == strBoneName)
										{
											return true;
										}
										iBoneIndex++;
										return false;
									});
								if (iter == BoneNames.end())
								{
									Log("Failed to Find matching name");
								}

								OutputFile.write(reinterpret_cast<const char*>(&iBoneIndex), sizeof(unsigned int));

								unsigned int iNumKeyFrame = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
								iNumKeyFrame = max(iNumKeyFrame, pChannel->mNumPositionKeys);
								OutputFile.write(reinterpret_cast<const char*>(&iNumKeyFrame), sizeof(unsigned int));

								float fTime{};
								float fScaleW{ 0.f };
								float fPositionW{ 1.f };

								for (size_t k = 0; k < iNumKeyFrame; k++)
								{
									if (k < pChannel->mNumScalingKeys)
									{
										OutputFile.write(reinterpret_cast<const char*>(&pChannel->mScalingKeys[k].mValue), sizeof(aiVector3D));
										OutputFile.write(reinterpret_cast<const char*>(&fScaleW), sizeof(float));

										fTime = static_cast<float>(pChannel->mScalingKeys[k].mTime);
									}
									if (k < pChannel->mNumRotationKeys)
									{
										OutputFile.write(reinterpret_cast<const char*>(&pChannel->mRotationKeys[k].mValue.x), sizeof(float));
										OutputFile.write(reinterpret_cast<const char*>(&pChannel->mRotationKeys[k].mValue.y), sizeof(float));
										OutputFile.write(reinterpret_cast<const char*>(&pChannel->mRotationKeys[k].mValue.z), sizeof(float));
										OutputFile.write(reinterpret_cast<const char*>(&pChannel->mRotationKeys[k].mValue.w), sizeof(float));

										fTime = static_cast<float>(pChannel->mRotationKeys[k].mTime);
									}
									if (k < pChannel->mNumPositionKeys)
									{
										OutputFile.write(reinterpret_cast<const char*>(&pChannel->mPositionKeys[k].mValue), sizeof(aiVector3D));
										OutputFile.write(reinterpret_cast<const char*>(&fPositionW), sizeof(float));

										fTime = static_cast<float>(pChannel->mPositionKeys[k].mTime);
									}

									OutputFile.write(reinterpret_cast<const char*>(&fTime), sizeof(float));
								}
							}
						}
					}
				#pragma endregion

					iNumAnimFiles++;
					OutputFile.close();
				}
			}
		}
	#pragma endregion

		Log("Animation Models : Convert Success!\n");
	}

	if (iConvertMode != 2)
	{
		Log(aiString(std::to_string(iNumStaticFiles) + " Static Models"));
	}
	if (iConvertMode != 1)
	{
		Log(aiString(std::to_string(iNumAnimFiles) + " Animation Models"));
	}
	Log("are Successfully Converted as hyuntramesh!!");

	LogFile.close();
	system("pause");
}

void Log(const aiString& Text)
{
	std::cout << Text.C_Str() << std::endl;
	LogFile << Text.C_Str() << std::endl;
}

void Log(const char* Text)
{

	std::cout << Text << std::endl;
	LogFile << Text << std::endl;
}

void Write_AnimMeshes(std::ofstream& OutputFile, const aiScene* pAIScene, const std::vector<aiString>& BoneNames, int VTFMode)
{
	OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mNumMeshes), sizeof(unsigned int));

	for (size_t i = 0; i < pAIScene->mNumMeshes; i++)
	{
		aiMesh* pMesh = pAIScene->mMeshes[i];

		unsigned int iMatIndex = pMesh->mMaterialIndex;
		aiString strName = pMesh->mName;
		unsigned int iNameSize = strName.length + 1;
		unsigned int iNumVertices = pMesh->mNumVertices;
		unsigned int iNumFaces = pMesh->mNumFaces;
		unsigned int iNumBones = pMesh->mNumBones;

		OutputFile.write(reinterpret_cast<const char*>(&iMatIndex), sizeof(unsigned int));
		OutputFile.write(reinterpret_cast<const char*>(&iNameSize), sizeof(unsigned int));
		OutputFile.write(strName.C_Str(), iNameSize);
		OutputFile.write(reinterpret_cast<const char*>(&iNumVertices), sizeof(unsigned int));
		OutputFile.write(reinterpret_cast<const char*>(&iNumFaces), sizeof(unsigned int));
		OutputFile.write(reinterpret_cast<const char*>(&iNumBones), sizeof(unsigned int));

		Uint4* vBlendIndices = new Uint4[iNumVertices]{};
		Float4* vBlendWeights = new Float4[iNumVertices]{};

		for (unsigned int j = 0; j < iNumBones; j++)
		{
			aiBone* pBone = pMesh->mBones[j];


			unsigned int BoneIndices = j;
			if (1 != VTFMode) {
				// For VTF Model
				unsigned int iBoneIndex{};
				auto iter = std::find_if(BoneNames.begin(), BoneNames.end(), [&pBone, &iBoneIndex](aiString strBoneName)
					{
						if (pBone->mName == strBoneName)
						{
							return true;
						}
						iBoneIndex++;
						return false;
					});
				if (iter == BoneNames.end())
				{
					std::cout << "Failed to Find matching name" << std::endl;
				}
				//
				BoneIndices = iBoneIndex;
			}

			for (unsigned int k = 0; k < pBone->mNumWeights; k++)
			{
				if (vBlendWeights[pBone->mWeights[k].mVertexId].x == 0.f)
				{
					vBlendIndices[pBone->mWeights[k].mVertexId].x = BoneIndices;
					vBlendWeights[pBone->mWeights[k].mVertexId].x = pBone->mWeights[k].mWeight;
				}
				else if (vBlendWeights[pBone->mWeights[k].mVertexId].y == 0.f)
				{
					vBlendIndices[pBone->mWeights[k].mVertexId].y = BoneIndices;
					vBlendWeights[pBone->mWeights[k].mVertexId].y = pBone->mWeights[k].mWeight;
				}
				else if (vBlendWeights[pBone->mWeights[k].mVertexId].z == 0.f)
				{
					vBlendIndices[pBone->mWeights[k].mVertexId].z = BoneIndices;
					vBlendWeights[pBone->mWeights[k].mVertexId].z = pBone->mWeights[k].mWeight;
				}
				else if (vBlendWeights[pBone->mWeights[k].mVertexId].w == 0.f)
				{
					vBlendIndices[pBone->mWeights[k].mVertexId].w = BoneIndices;
					vBlendWeights[pBone->mWeights[k].mVertexId].w = pBone->mWeights[k].mWeight;
				}
			}
		}

	#pragma region 노말 직접 계산
		//unsigned int* Indices = new unsigned int[pMesh->mNumFaces * 3] {};
//unsigned int Index{};

//XMFLOAT4* pNormals = new XMFLOAT4[iNumVertices]{};

//for (size_t j = 0; j < iNumFaces; j++)
//{
//	Indices[Index++] = pMesh->mFaces[j].mIndices[0];
//	Indices[Index++] = pMesh->mFaces[j].mIndices[1];
//	Indices[Index++] = pMesh->mFaces[j].mIndices[2];

//	aiVector3D vSour = pMesh->mVertices[pMesh->mFaces[j].mIndices[1]] - pMesh->mVertices[pMesh->mFaces[j].mIndices[0]];
//	aiVector3D vDest = pMesh->mVertices[pMesh->mFaces[j].mIndices[2]] - pMesh->mVertices[pMesh->mFaces[j].mIndices[1]];
//	XMVECTOR vSrc = XMVectorSet(vSour.x, vSour.y, vSour.z, 1.f);
//	XMVECTOR vDst = XMVectorSet(vDest.x, vDest.y, vDest.z, 1.f);
//	XMVECTOR vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

//	XMStoreFloat4(&pNormals[pMesh->mFaces[j].mIndices[0]], XMLoadFloat4(&pNormals[pMesh->mFaces[j].mIndices[0]]) + vNor);
//	XMStoreFloat4(&pNormals[pMesh->mFaces[j].mIndices[1]], XMLoadFloat4(&pNormals[pMesh->mFaces[j].mIndices[1]]) + vNor);
//	XMStoreFloat4(&pNormals[pMesh->mFaces[j].mIndices[2]], XMLoadFloat4(&pNormals[pMesh->mFaces[j].mIndices[2]]) + vNor);

//}  
	#pragma endregion

		for (size_t j = 0; j < iNumVertices; j++)
		{
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mVertices[j]), sizeof aiVector3D);
			//OutputFile.write(reinterpret_cast<const char*>(&pNormals[j]), sizeof aiVector3D);
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mNormals[j]), sizeof aiVector3D);
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mTextureCoords[0][j]), sizeof aiVector2D);
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mTangents[j]), sizeof aiVector3D);
			OutputFile.write(reinterpret_cast<const char*>(&vBlendIndices[j]), sizeof Uint4);
			OutputFile.write(reinterpret_cast<const char*>(&vBlendWeights[j]), sizeof Float4);
		}

		delete[] vBlendIndices;
		delete[] vBlendWeights;

		for (size_t j = 0; j < iNumBones; j++)
		{
			aiBone* pBone = pMesh->mBones[j];

			OutputFile.write(reinterpret_cast<const char*>(&pBone->mOffsetMatrix.Transpose()), sizeof aiMatrix4x4);

			unsigned int iBoneIndex{};
			auto iter = std::find_if(BoneNames.begin(), BoneNames.end(), [&pBone, &iBoneIndex](aiString strBoneName)
			{
				if (pBone->mName == strBoneName)
				{
					return true;
				}
				iBoneIndex++;
				return false;
			});
			if (iter == BoneNames.end())
			{
				std::cout << "Failed to Find matching name" << std::endl;
			}

			OutputFile.write(reinterpret_cast<const char*>(&iBoneIndex), sizeof(unsigned int));
		}

		for (size_t j = 0; j < iNumFaces; j++)
		{
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[0]), sizeof(unsigned int));
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[1]), sizeof(unsigned int));
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[2]), sizeof(unsigned int));
		}
	}
}

void Write_StatMeshes(std::ofstream& OutputFile, const aiScene* pAIScene)
{
	OutputFile.write(reinterpret_cast<const char*>(&pAIScene->mNumMeshes), sizeof(unsigned int));

	for (size_t i = 0; i < pAIScene->mNumMeshes; i++)
	{
		aiMesh* pMesh = pAIScene->mMeshes[i];

		unsigned int iMatIndex = pMesh->mMaterialIndex;
		std::string strName = pMesh->mName.C_Str();
		unsigned int iNameSize = strName.size() + 1;
		unsigned int iNumVertices = pMesh->mNumVertices;
		unsigned int iNumFaces = pMesh->mNumFaces;

		OutputFile.write(reinterpret_cast<const char*>(&iMatIndex), sizeof(unsigned int));
		OutputFile.write(reinterpret_cast<const char*>(&iNameSize), sizeof(unsigned int));
		OutputFile.write(strName.c_str(), iNameSize);
		OutputFile.write(reinterpret_cast<const char*>(&iNumVertices), sizeof(unsigned int));
		OutputFile.write(reinterpret_cast<const char*>(&iNumFaces), sizeof(unsigned int));

		for (size_t j = 0; j < iNumVertices; j++)
		{
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mVertices[j]), sizeof aiVector3D);
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mNormals[j]), sizeof aiVector3D);
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mTextureCoords[0][j]), sizeof aiVector2D);
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mTangents[j]), sizeof aiVector3D);
		}

		for (size_t j = 0; j < iNumFaces; j++)
		{
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[0]), sizeof(unsigned int));
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[1]), sizeof(unsigned int));
			OutputFile.write(reinterpret_cast<const char*>(&pMesh->mFaces[j].mIndices[2]), sizeof(unsigned int));
		}
	}
}

void Write_Materials(std::ofstream& OutputFile, const aiScene* pAIScene)
{
	unsigned int NumMaterials = pAIScene->mNumMaterials;
	OutputFile.write(reinterpret_cast<const char*>(&NumMaterials), sizeof(unsigned int));

	for (size_t i = 0; i < NumMaterials; i++)
	{
		aiMaterial* pAIMaterial = pAIScene->mMaterials[i];
		unsigned int iTexturePathNameSize{};

		for (size_t j = 0; j < 18; j++)
		{
			aiString strTexturePath{};
			pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath);
			char TextureFileName[_MAX_PATH]{};
			char TextureExt[_MAX_PATH]{};
			_splitpath_s(strTexturePath.C_Str(), nullptr, 0, nullptr, 0, TextureFileName, _MAX_PATH, TextureExt, _MAX_PATH);

			strcat_s(TextureFileName, TextureExt);
			iTexturePathNameSize = static_cast<unsigned int>(strlen(TextureFileName)) + 1;
			OutputFile.write(reinterpret_cast<const char*>(&iTexturePathNameSize), sizeof(unsigned int));
			if (iTexturePathNameSize == 1)
			{
				continue;
			}
			OutputFile.write(TextureFileName, iTexturePathNameSize);
		}
	}
}
