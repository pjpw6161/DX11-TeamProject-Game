#pragma once

#include "Client_Define.h"
#include "NPC.h"

BEGIN(Client)

enum NPC_TYPE
{
	TYPE_END
};

class CNPC_Dummy final : public CNPC
{
private:
	CNPC_Dummy(_dev pDevice, _context pContext);
	CNPC_Dummy(const CNPC_Dummy& rhs);
	virtual ~CNPC_Dummy() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	NPC_TYPE m_eNPCType{ TYPE_END };

	CGameObject* m_pDialog{ nullptr };
	_float m_fTimer{};

public:
	static CNPC_Dummy* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

#pragma region NPC_Dummy Anim List

//Converting... Dwarf_Male_001.fbx
//Read File Succssefully
//Animation name list :
//NPC_Dwarf_male_001.ao | idle
//NPC_Dwarf_male_001.ao | idle02
//NPC_Dwarf_male_001.ao | Reaction01
//NPC_Dwarf_male_001.ao | Reaction02
//NPC_Dwarf_male_001.ao | Talk01
//NPC_Dwarf_male_001.ao | walk01
//Converting... Dwarf_Male_002.fbx
//Read File Succssefully
//Animation name list :
//NPC_Dwarf_male_002.ao | Idle01
//NPC_Dwarf_male_002.ao | talk01
//NPC_Dwarf_male_002.ao | walk
//Converting... Female_003.fbx
//Read File Succssefully
//Animation name list :
//NPC_female_003.ao | Cheer01
//NPC_female_003.ao | Emo01
//NPC_female_003.ao | idle01
//NPC_female_003.ao | idle02
//NPC_female_003.ao | idle02_end
//NPC_female_003.ao | idle02_loop
//NPC_female_003.ao | idle02_start
//NPC_female_003.ao | idle03
//NPC_female_003.ao | pose01
//NPC_female_003.ao | pose02
//NPC_female_003.ao | pose03
//NPC_female_003.ao | pose04
//NPC_female_003.ao | pose05
//NPC_female_003.ao | Sit_Talk01
//NPC_female_003.ao | sitIdle_01
//NPC_female_003.ao | sitIdle_02
//NPC_female_003.ao | talk01
//NPC_female_003.ao | talk02
//NPC_female_003.ao | talk03
//NPC_female_003.ao | walk01
//Converting... Female_004.fbx
//Read File Succssefully
//Animation name list :
//NPC_female_004.ao | Cheer
//NPC_female_004.ao | Emo01
//NPC_female_004.ao | Idle01
//NPC_female_004.ao | Idle02
//NPC_female_004.ao | Reaction02
//NPC_female_004.ao | Sit_Talk01
//NPC_female_004.ao | sitIdle_01
//NPC_female_004.ao | sitIdle_02
//NPC_female_004.ao | Talk01
//NPC_female_004.ao | Talk02
//NPC_female_004.ao | Walk01
//NPC_female_004.ao | Walk02
//Converting... Female_006.fbx
//Read File Succssefully
//Animation name list :
//NPC_female_006.ao | Cheer
//NPC_female_006.ao | Emo01
//NPC_female_006.ao | Emo02
//NPC_female_006.ao | Emo03
//NPC_female_006.ao | idle01
//NPC_female_006.ao | idle02
//NPC_female_006.ao | idle02_End
//NPC_female_006.ao | idle02_loop
//NPC_female_006.ao | idle02_Start
//NPC_female_006.ao | nurse_idle01
//NPC_female_006.ao | nurse_idle02
//NPC_female_006.ao | nurse_idle03
//NPC_female_006.ao | pose01
//NPC_female_006.ao | pose02
//NPC_female_006.ao | pose03
//NPC_female_006.ao | pose04
//NPC_female_006.ao | pose05
//NPC_female_006.ao | Reaction01
//NPC_female_006.ao | talk01
//NPC_female_006.ao | talk02
//NPC_female_006.ao | talk03
//NPC_female_006.ao | walk01
//Converting... Female_013.fbx
//Read File Succssefully
//Animation name list :
//NPC_female_013.ao | Emo01
//NPC_female_013.ao | Emo02
//NPC_female_013.ao | Idle01
//NPC_female_013.ao | Talk01
//NPC_female_013.ao | Walk01
//Converting... Female_027.fbx
//Read File Succssefully
//Animation name list :
//NPC_female_027.ao | Cheer
//NPC_female_027.ao | Emo01
//NPC_female_027.ao | Emo02
//NPC_female_027.ao | Emo03
//NPC_female_027.ao | idle01
//NPC_female_027.ao | idle02
//NPC_female_027.ao | idle02_End
//NPC_female_027.ao | idle02_loop
//NPC_female_027.ao | idle02_Start
//NPC_female_027.ao | nurse_idle01
//NPC_female_027.ao | nurse_idle02
//NPC_female_027.ao | nurse_idle03
//NPC_female_027.ao | pose01
//NPC_female_027.ao | pose02
//NPC_female_027.ao | pose03
//NPC_female_027.ao | pose04
//NPC_female_027.ao | pose05
//NPC_female_027.ao | Reaction01
//NPC_female_027.ao | talk01
//NPC_female_027.ao | talk02
//NPC_female_027.ao | talk03
//NPC_female_027.ao | walk01
//Converting... Male_013.fbx
//Read File Succssefully
//Animation name list :
//NPC_Male_013.ao | Emo01
//NPC_Male_013.ao | idle01
//NPC_Male_013.ao | idle02
//NPC_Male_013.ao | talk01
//NPC_Male_013.ao | talk02
//NPC_Male_013.ao | talk03
//NPC_Male_013.ao | walk
//Converting... Male_015.fbx
//Read File Succssefully
//Animation name list :
//NPC_Male_015.ao | Emo01
//NPC_Male_015.ao | Fire
//NPC_Male_015.ao | Idle01
//NPC_Male_015.ao | Idle02
//NPC_Male_015.ao | Reaction01
//NPC_Male_015.ao | Sit01
//NPC_Male_015.ao | Talk01
//NPC_Male_015.ao | Talk02
//NPC_Male_015.ao | Tu02_SC04_NPC_01
//NPC_Male_015.ao | Walk
//Converting... Male_016.fbx
//Read File Succssefully
//Animation name list :
//NPC_male_016.ao | Dance01
//NPC_male_016.ao | Dance02
//NPC_male_016.ao | Emo01
//NPC_male_016.ao | idle
//NPC_male_016.ao | Panic
//NPC_male_016.ao | Reaction02
//NPC_male_016.ao | talk
//NPC_male_016.ao | walk
//Converting... Male_018.fbx
//Read File Succssefully
//Animation name list :
//NPC_male_018.ao | Emo01
//NPC_male_018.ao | Idle01
//NPC_male_018.ao | Idle02
//NPC_male_018.ao | Idle03
//NPC_male_018.ao | Reaction01
//NPC_male_018.ao | Talk01
//NPC_male_018.ao | Talk02
//NPC_male_018.ao | Talk03
//NPC_male_018.ao | Walk
//Converting... Male_020.fbx
//Read File Succssefully
//Animation name list :
//NPC_Male_020.ao | Emo01
//NPC_Male_020.ao | Idle
//NPC_Male_020.ao | Talk
//NPC_Male_020.ao | Walk
//Converting... Male_027.fbx
//Read File Succssefully
//Animation name list :
//Npc_male_027.ao | Idle
//Npc_male_027.ao | Play
//Converting... Male_Chi.fbx
//Read File Succssefully
//Animation name list :
//NPC_male_004.ao | Cheer
//NPC_male_004.ao | Complain
//NPC_male_004.ao | Emo01
//NPC_male_004.ao | Emo02
//NPC_male_004.ao | Idle01
//NPC_male_004.ao | Idle02
//NPC_male_004.ao | Idle03
//NPC_male_004.ao | Talk01
//NPC_male_004.ao | Talk02
//NPC_male_004.ao | Talk03
//NPC_male_004.ao | Walk01
//NPC_male_004.ao | Walk02
//NPC_male_004.ao | Walk03
//Converting... Roskva.fbx
//Read File Succssefully
//Animation name list :
//NPC_Roskva_re.ao | Coast02_SC02_NPC_Roskva
//NPC_Roskva_re.ao | Idle01
//NPC_Roskva_re.ao | LoadingScene_Pose_roskva
//NPC_Roskva_re.ao | Talk01_old
//NPC_Roskva_re.ao | Talk02
//NPC_Roskva_re.ao | Talk03
//NPC_Roskva_re.ao | Walk
//Animation Models : Convert Success!

// Ãß°¡
//Converting... Female_002.fbx
//Read File Succssefully
//Animation name list :
//NPC_female_002.ao | Cheer
//NPC_female_002.ao | Emo01
//NPC_female_002.ao | Emo02
//NPC_female_002.ao | Idle01
//NPC_female_002.ao | Idle02
//NPC_female_002.ao | Reaction01
//NPC_female_002.ao | Sit_Talk01
//NPC_female_002.ao | Talk01
//NPC_female_002.ao | Talk02
//NPC_female_002.ao | Walk
//Converting... Female_005.fbx
//Read File Succssefully
//Animation name list :
//NPC_female_005.ao | Cheer01
//NPC_female_005.ao | Emo01
//NPC_female_005.ao | Emo02
//NPC_female_005.ao | Idle01
//NPC_female_005.ao | Idle02
//NPC_female_005.ao | Sit_Talk01
//NPC_female_005.ao | Talk01
//NPC_female_005.ao | Talk02
//NPC_female_005.ao | Walk
//Converting... Female_010.fbx
//Read File Succssefully
//Animation name list :
//NPC_female_010.ao | Cheer
//NPC_female_010.ao | Emo01
//NPC_female_010.ao | Emo02
//NPC_female_010.ao | Idle01
//NPC_female_010.ao | Idle02
//NPC_female_010.ao | nurse_idle01
//NPC_female_010.ao | nurse_idle02
//NPC_female_010.ao | nurse_idle03
//NPC_female_010.ao | Sit01
//NPC_female_010.ao | Talk01
//NPC_female_010.ao | Talk02
//NPC_female_010.ao | Walk
//Converting... Horse.fbx
//Read File Succssefully
//Animation name list :
//NPC_Animal_12.ao | idle01
//NPC_Animal_12.ao | idle02
//NPC_Animal_12.ao | run
//NPC_Animal_12.ao | walk
//Converting... Male_009.fbx
//Read File Succssefully
//Animation name list :
//NPC_male_009.ao | Emo01
//NPC_male_009.ao | idle01
//NPC_male_009.ao | idle_02
//NPC_male_009.ao | talk01
//NPC_male_009.ao | talk02
//NPC_male_009.ao | talk03
//NPC_male_009.ao | walk
//Animation Models : Convert Success!

#pragma endregion NPC_Dummy Anim List
