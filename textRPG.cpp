#include <iostream>
#include <time.h>
#include <string.h>

#define NAME_SIZE 32
#define ITEM_DESC_LENGTH 512
#define INVENTORY_MAX 20
#define STORE_WEAPON_MAX 3
#define STORE_ARMOR_MAX 3
#define LEVEL_MAX 10

using namespace std;

// 레벨업에 필요한 경험치 목록
const int g_LevelUpExp[LEVEL_MAX] = {4000, 10000, 20000, 35000, 50000, 70000, 100000, 150000, 200000, 400000};

enum MAIN_MENU
{
    MM_NONE,
    MM_MAP,
    MM_STORE,
    MM_INVENTORY,
    MM_EXIT
};

enum MAP_TYPE
{
    MT_NONE,
    MT_EASY,
    MT_NORMAL,
    MT_HARD,
    MT_BACK
};

enum JOB
{
    JOB_NONE,
    JOB_KNIGHT,
    JOB_ARCHER,
    JOB_WIZARD,
    JOB_END         // 직업 추가의 편리성을 위해
};

enum EQUIP
{
    EQ_WEAPON,
    EQ_ARMOR,
    EQ_MAX
};

enum ITEM_TYPE
{
    IT_NONE,
    IT_WEAPON,
    IT_ARMOR,
    IT_BACK
};

enum BATTLE
{
    BATTLE_NONE,
    BATTLE_ATTACK,
    BATTLE_BACK
};

enum STORE_MENU
{
    SM_NONE,
    SM_WEAPON,
    SM_ARMOR,
    SM_BACK
};

typedef struct _tagItem
{
    char strName[NAME_SIZE];
    char strTypeName[NAME_SIZE];
    ITEM_TYPE eType;
    int iMin;
    int iMax;
    int iPrice;
    int iSell;
    char strDesc[ITEM_DESC_LENGTH];
} TagItem;

typedef TagItem tItem;

typedef struct _tagInventory
{
    _tagItem tItem[INVENTORY_MAX];
    int iItemCount;
    int iGold;
} TagInventory;

typedef TagInventory tInventory;

typedef struct _tagPlayer
{
    char strName[NAME_SIZE];
    char strJobName[NAME_SIZE];
    JOB eJob;
    int iAttackMin;
    int iAttackMax;
    int iArmorMin;
    int iArmorMax;
    int iHP;
    int iHPMax;
    int iMP;
    int iMPMax;
    int iExp;
    int iLevel;
    bool bEquip[EQ_MAX];
    tItem tEquip[EQ_MAX];
    tInventory tInventory;
} TagPlayer;

typedef TagPlayer tPlayer;

typedef struct _tagMonster
{
    char strName[NAME_SIZE];
    int iAttackMin;
    int iAttackMax;
    int iArmorMin;
    int iArmorMax;
    int iHP;
    int iHPMax;
    int iMP;
    int iMPMax;
    int iLevel;
    int iExp;
    int iGoldMin;
    int iGoldMax;
} TagMonster;

typedef TagMonster tMonster;

typedef struct _tagLevelUpStatus
{
    int iAttackMin;
    int iAttackMax;
    int iArmorMin;
    int iArmorMax;
    int iHPMin;
    int iHPMax;
    int iMPMin;
    int iMPMax;
} TagLevelUpStatus;

typedef TagLevelUpStatus tLvUpStatus;
tLvUpStatus g_LvUpTable[JOB_END - 1] = {};

int InputInt();
int OutputMainMenu();
void OutputBattleTag(int iMenu);
void OutputPlayer(tPlayer *pPlayer);
void OutputMonster(tMonster *pMonster);
int OutputBattleMenu();
void RunMap(tPlayer *pPlayer, tMonster *pMonsterArr);
int OutputMapMenu();
void Battle(tPlayer *pPlayer, tMonster *pMonster);
void RunBattle(tPlayer *pPlayer, tMonster *pMonsterArr, int iMenu);
void SetPlayer(tPlayer *pPlayer);
int SelectJob();
tMonster CreateMonster(char *pName, int iAttackMin, 
    int iAttackMax, int iArmorMin, int iArmorMax, int iHP,
    int iMP, int iLevel, int iExp, int iGoldMin, int iGoldMax);
void SetMonster(tMonster *pMonsterArr);
tLvUpStatus CreateLvUpStatus(int iAttackMin, int iAttackMax,
    int iArmorMin, int iArmorMax, int iHPMin, int iHPMax, 
    int iMPMin, int iMPMax);
int OutputStoreMenu();
int OutputStoreItemList(tInventory *pInventory, tItem *pStore, 
    int iItemCount);
void BuyItem(tInventory *pInventory, tItem *pStore,
    int iItemCount, int iStoreType);
void RunStore(tInventory *pInventory, tItem *pWeapon, 
    tItem *pArmor);
tItem CreateItem(char *pName, ITEM_TYPE eType, 
    int iMin, int iMax, int iPrice, int iSell, char *pDesc);
int OutputInventory(tPlayer *pPlayer);
EQUIP ComputeEquipType(ITEM_TYPE eType);
void RunInventory(tPlayer *pPlayer);

// 메인 함수
int main(void)
{
    srand((unsigned int)time(0));

    // 게임 시작할 때 플레이어 정보 설정
    tPlayer tPlayer = {};

    // 플레이어 정보 정의
    SetPlayer(&tPlayer);

    // 몬스터 생성
    tMonster tMonsterArr[MT_BACK - 1] = {};
    SetMonster(tMonsterArr);

    // 기사 초기 능력치
    g_LvUpTable[JOB_KNIGHT - 1] = CreateLvUpStatus(4, 10, 8, 16, 50,
        100, 10, 20);
    
    // 궁수 초기 능력치
    g_LvUpTable[JOB_ARCHER - 1] = CreateLvUpStatus(10, 15, 5, 10, 30,
        60, 30, 50);
    
    // 마법사 초기 능력치
    g_LvUpTable[JOB_WIZARD - 1] = CreateLvUpStatus(15, 20, 3, 7, 30,
        40, 50, 100);

    // 상점에서 판매할 아이템 목록 생성
    tItem tStoreWeapon[STORE_WEAPON_MAX] = {};
    tItem tStoreArmor[STORE_ARMOR_MAX] = {};

    tStoreWeapon[0] = CreateItem("목검", IT_WEAPON, 5, 10,
        1000, 500, "나무로 만든 검");
    tStoreWeapon[1] = CreateItem("장궁", IT_WEAPON, 20, 40,
        7000, 3500, "나무로 만든 활");
    tStoreWeapon[2] = CreateItem("지팡이", IT_WEAPON, 90, 150,
        30000, 15000, "나무로 만든 지팡이");

    tStoreArmor[0] = CreateItem("로브", IT_ARMOR, 2, 5,
        1000, 500, "천으로 만든 로브");
    tStoreArmor[1] = CreateItem("가죽갑옷", IT_ARMOR, 10, 20,
        7000, 3500, "가죽으로 만든 질긴 갑옷");
    tStoreArmor[2] = CreateItem("판금갑옷", IT_ARMOR, 70, 90,
        10000, 5000, "기사들이 입는 갑옷");

    bool bLoop = true;
    
    while (bLoop) {
        switch (OutputMainMenu()) {
            case MM_MAP:
                RunMap(&tPlayer, tMonsterArr);   // 맵 관련 루프 처리
                break;
            case MM_STORE:
                RunStore(&tPlayer.tInventory, tStoreWeapon, tStoreArmor);
                break;
            case MM_INVENTORY:
                RunInventory(&tPlayer);
                break;
            case MM_EXIT:
                bLoop = false;
                break;
        }
    }
    return 0;
}


// 숫자 입력을 받는다. 입력 오류까지 처리해 주고
// INT_MAX를 리턴하면 입력 오류이다
int InputInt()
{
    int iInput;
    cin >> iInput;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(1024, '\n');
        return INT_MAX;     // 잘못 입력됐음을 의미
    }

    return iInput;
}

// 메인 메뉴를 화면에 보여주고 입력받은 메뉴를 반환함
// 입력 오류일 경우 혹은 잘못된 번호를 입력한 경우 MM_NONE 반환
int OutputMainMenu()
{
    system("cls");
    cout << "****************** 로비 *******************" << endl;
    cout << "1. 맵" << endl;
    cout << "2. 상점" << endl;
    cout << "3. 가방" << endl;
    cout << "4. 종료" << endl;
    cout << "메뉴를 선택하세요: ";

    int iMenu = InputInt();

    if (iMenu == INT_MAX || iMenu <= MM_NONE || iMenu > MM_EXIT)
        return MM_NONE;
    
    return iMenu;
}

void OutputBattleTag(int iMenu)
{
    switch (iMenu) {
        case MT_EASY:
            cout << "****************** 쉬움 *******************" << endl;
            break;
        case MT_NORMAL:
            cout << "****************** 중간 *******************" << endl;
            break;
        case MT_HARD:
            cout << "****************** 어려움 *******************" << endl;
            break;
    }
}

void OutputPlayer(tPlayer *pPlayer)
{
    // 플레이어 정보 출력
    cout << "================= Player =================" << endl;
    cout << "이름: " << pPlayer->strName << "\n직업: "
        << pPlayer->strJobName << endl;
    cout << "레벨: " << pPlayer->iLevel  << "\t경험치: "
        << pPlayer->iExp << " / " << g_LevelUpExp[pPlayer->iLevel - 1] <<endl;

    // 무기를 장착했을 경우 공격력에 무기 공격력을 추가하여 출력한다
    if (pPlayer->bEquip[EQ_WEAPON] == true) {
        cout << "공격력: " << pPlayer->iAttackMin << " + " << pPlayer->tEquip[EQ_WEAPON].iMin << " - " <<
        pPlayer->iAttackMax << " + " << pPlayer->tEquip[EQ_WEAPON].iMax;
    }
    else 
        cout << "공격력: " << pPlayer->iAttackMin << " - " << pPlayer->iAttackMax;
    // 방어구를 장착했을 경우 방어력에 방어구의 방어력을 추가하여 출력한다
    if (pPlayer->bEquip[EQ_ARMOR] == true) {
        cout << "\t방어력: " << pPlayer->iArmorMin << " + " << pPlayer->tEquip[EQ_ARMOR].iMin << " - " <<
        pPlayer->iArmorMax << " + " << pPlayer->tEquip[EQ_ARMOR].iMax << endl;
    }
    else 
        cout << "\t방어력: " << pPlayer->iArmorMin << " - " << pPlayer->iArmorMax << endl;
    
    cout << "체력: " << pPlayer->iHP << " / " << pPlayer->iHPMax <<
            "\t마나: " << pPlayer->iMP << " / " << pPlayer->iMPMax << endl;
    
    if (pPlayer->bEquip[EQ_WEAPON] == true)
        cout << "장착무기: " << pPlayer->tEquip[EQ_WEAPON].strName;
    else
        cout << "장착무기: 없음";

    if (pPlayer->bEquip[EQ_ARMOR] == true)
        cout << "\t장착방어구: " << pPlayer->tEquip[EQ_ARMOR].strName << endl;
    else
        cout << "\t장착방어구: 없음" << endl;
    
    cout << "보유골드: " << pPlayer->tInventory.iGold << " Gold" << endl << endl;
}

void OutputMonster(tMonster *pMonster)
{
    // 몬스터 정보 출력
    cout << "================= Monster =================" << endl;
    cout << "이름: " << pMonster->strName << "\n레벨: "
        << pMonster->iLevel << endl;
    cout << "공격력: " << pMonster->iArmorMin << " - " <<
        pMonster->iAttackMax << "\t방어력: " << pMonster->iArmorMin <<
        " - " << pMonster->iArmorMax << endl;
    cout << "체력: " << pMonster->iHP << " / " << pMonster->iHPMax <<
        "\t마나: " << pMonster->iMP << " / " << pMonster->iMPMax << endl;
    cout << "획득 경험치: " << pMonster->iExp << "\t획득 골드: " <<
        pMonster->iGoldMin << " - " << pMonster->iGoldMax << endl << endl;
}

int OutputBattleMenu()
{
    cout << "1. 공격" << endl;
    cout << "2. 도망" << endl;
    cout << "메뉴를 선택하세요: ";
    int iMenu = InputInt();

    if (iMenu == INT_MAX || iMenu <= BATTLE_NONE || iMenu > BATTLE_BACK)
        return BATTLE_NONE;

    return iMenu;
}

void Battle(tPlayer *pPlayer, tMonster *pMonster)
{
    // 예를 들어 min = 5, max = 15 이라고 가정할 경우
    // 15 - 5 + 1 을 하면 11이 된다. 11로 나눈 나머지는 0 ~ 10 이고
    // 여기에 min = 5를 더하면 5 ~ 15 사이로 값이 나오게 된다
    int iAttackMin = pPlayer->iAttackMin;
    int iAttackMax = pPlayer->iAttackMax;
    // 무기를 장착하고 있을 경우 무기와 Min, Max를 더해준다
    if (pPlayer->bEquip[EQ_WEAPON]) {
        iAttackMin += pPlayer->tEquip[EQ_WEAPON].iMin;
        iAttackMax += pPlayer->tEquip[EQ_WEAPON].iMax;
    }

    int iAttack = rand() % (iAttackMax - iAttackMin + 1) +
        iAttackMin;
    int iArmor = rand() % (pMonster->iArmorMax - pMonster->iArmorMin + 1) +
        pMonster->iArmorMin;
    int iDamage = iAttack - iArmor;
    // 삼항연산자: 조건식 ? true일 때 값 : false일 때 값
    iDamage = (iDamage < 1) ? 1 : iDamage;
    
    // 몬스터 HP를 감소시킨다
    pMonster->iHP -= iDamage;
    cout << pPlayer->strName << " 이(가) " << pMonster->strName << " 에게 " <<
        iDamage << " 피해를 입혔습니다." << endl;
    
    // 몬스터가 죽었을 경우
    if (pMonster->iHP <= 0) {
        cout << pMonster->strName << " 이(가) 사망했습니다." << endl;
        pPlayer->iExp += pMonster->iExp;
        int iGold = (rand() % (pMonster->iGoldMax - pMonster->iGoldMin + 1) +
            pMonster->iGoldMin);
        pPlayer->tInventory.iGold += iGold;
        cout << pMonster->iExp << " 경험치를 획득하였습니다." << endl;
        cout << iGold << " Gold를 획득하였습니다." << endl;

        pMonster->iHP = pMonster->iHPMax;
        pMonster->iMP = pMonster->iMPMax;

        // 레벨업을 했는지 체크
        if (pPlayer->iExp >= g_LevelUpExp[pPlayer->iLevel - 1]) {
            // 플레이어 경험치를 레벨업에 필요한 경험치만큼 차감
            pPlayer->iExp -= g_LevelUpExp[pPlayer->iLevel - 1];

            // 레벨업
            pPlayer->iLevel++;
            cout << "LV." << pPlayer->iLevel << " 로 레벨업 했습니다." << endl;

            // 능력치 상승
            // 직업 인덱스를 구한다
            int iJobIndex = pPlayer->eJob - 1;
            int iAttackUp = rand() % (g_LvUpTable[iJobIndex].iAttackMax - g_LvUpTable[iJobIndex].iAttackMin + 1) +
                g_LvUpTable[iJobIndex].iAttackMin;
            int iArmorUp = rand() % (g_LvUpTable[iJobIndex].iArmorMax - g_LvUpTable[iJobIndex].iArmorMin + 1) +
                g_LvUpTable[iJobIndex].iArmorMin;
            int iHPUp = rand() % (g_LvUpTable[iJobIndex].iHPMax - g_LvUpTable[iJobIndex].iHPMin + 1) +
                g_LvUpTable[iJobIndex].iHPMin;
            int iMPUp = rand() % (g_LvUpTable[iJobIndex].iMPMax - g_LvUpTable[iJobIndex].iMPMin + 1) +
                g_LvUpTable[iJobIndex].iMPMin;

            pPlayer->iAttackMin += g_LvUpTable[iJobIndex].iAttackMin;
            pPlayer->iAttackMax += g_LvUpTable[iJobIndex].iAttackMax;
            pPlayer->iArmorMin += g_LvUpTable[iJobIndex].iArmorMin;
            pPlayer->iArmorMax += g_LvUpTable[iJobIndex].iArmorMax;
            pPlayer->iHPMax += iHPUp;
            pPlayer->iMPMax += iMPUp;

            // 체력과 마나 회복
            pPlayer->iHP = pPlayer->iHPMax;
            pPlayer->iMP = pPlayer->iMPMax;
        }

        return;
    }

    // 몬스터가 살아있다면 플레이어를 공격한다
   iAttack = rand() % (pMonster->iAttackMax - pMonster->iAttackMin + 1) +
       pMonster->iAttackMin ;
   
   int iArmorMin = pPlayer->iArmorMin;
   int iArmorMax = pPlayer->iArmorMax;
   if (pPlayer->bEquip[EQ_ARMOR]) {
       iArmorMin += pPlayer->tEquip[EQ_ARMOR].iMin;
       iArmorMax += pPlayer->tEquip[EQ_ARMOR].iMax;
   }
   iArmor = rand() % (iArmorMax - iArmorMin + 1) +
       iArmorMin;
   iDamage = iAttack - iArmor;
   // 삼항연산자: 조건식 ? true일 때 값 : false일 때 값
   iDamage = (iDamage < 1) ? 1 : iDamage;
   
   // 플레이어 HP를 감소시킨다
   pPlayer->iHP -= iDamage;
    cout << pMonster->strName << " 이(가) " << pPlayer->strName << " 에게 " <<
        iDamage << " 피해를 입혔습니다." << endl;
    
    // 플레이어가 죽었을 경우
    if (pPlayer->iHP <= 0) {
        cout << pPlayer->strName << " 이(가) 사망했습니다." << endl;
        int iExp = pPlayer->iExp * 0.1f;
        int iGold = pPlayer->tInventory.iGold * 0.1f;
        pPlayer->iExp -= iExp;
        pPlayer->tInventory.iGold -= iGold;
        cout << iExp << " 경험치를 잃었습니다." << endl;
        cout << iGold << " 골드를 잃었습니다." << endl;
        // 플레이어의 HP와 MP를 회복한다.
        pPlayer->iHP = pPlayer->iHPMax;
        pPlayer->iMP = pPlayer->iMPMax;
    }
    system("pause");
}

void RunBattle(tPlayer *pPlayer, tMonster *pMonsterArr, int iMenu)
{
    tMonster tMonster = pMonsterArr[iMenu - 1];

    while (true) {
        system("cls");
        OutputBattleTag(iMenu);

        // 플레이어 출력
        OutputPlayer(pPlayer);

        // 몬스터 출력
        OutputMonster(&tMonster);

        switch (OutputBattleMenu()) {
            case BATTLE_ATTACK:
                Battle(pPlayer, &tMonster);
                break;
            case BATTLE_BACK:
                return;
        }
    }
}

int OutputMapMenu()
{
    system("cls");
    cout << "****************** 맵 *******************" << endl;
    cout << "1. 쉬움" << endl;
    cout << "2. 보통" << endl;
    cout << "3. 어려움" << endl;
    cout << "4. 뒤로가기" << endl;
    cout << "맵을 선택하세요: ";
    
    int iMenu = InputInt();

    if (iMenu == INT_MAX || iMenu <= MT_NONE || iMenu > MT_BACK)
        return MM_NONE;
    
    return iMenu;
}
// 맵에 관련된 동작 처리
void RunMap(tPlayer *pPlayer, tMonster *pMonsterArr)
{
    bool bLoop = true;

    while (bLoop) {
        int iMenu = OutputMapMenu();

        if (MT_BACK == iMenu)
            return;
        
        // 전투를 시작한다
        RunBattle(pPlayer, pMonsterArr, iMenu);
    }
}

int SelectJob()
{
    int iJob = JOB_NONE;
    
    while (iJob == JOB_NONE) {
        system("cls");
        cout << "1. 기사" << endl;
        cout << "2. 궁수" << endl;
        cout << "3. 마법사" << endl;
        cout << "직업을 선택하세요: ";
        iJob = InputInt();

        if (iJob <= JOB_NONE || iJob >= JOB_END)
            iJob = JOB_NONE;
    }

    return iJob;
}

void SetPlayer(tPlayer *pPlayer)
{
    // 플레이어의 이름 입력받는다
    cout << "이름을 설정하세요: ";
    cin.getline(pPlayer->strName, NAME_SIZE - 1);    // NULL 문자 확보를 위해
    
    pPlayer->eJob = (JOB)SelectJob();
    pPlayer->iLevel = 1;
    pPlayer->iExp = 0;
    pPlayer->tInventory.iItemCount = 0;
    pPlayer->tInventory.iGold = 10000;
    
    switch (pPlayer->eJob) {
        case JOB_KNIGHT:
            strcpy_s(pPlayer->strJobName, "기사");
            pPlayer->iAttackMin = 5;
            pPlayer->iAttackMax = 10;
            pPlayer->iArmorMin = 15;
            pPlayer->iArmorMax = 20;
            pPlayer->iHPMax = 500;
            pPlayer->iHP = 500;
            pPlayer->iMP = 100;
            pPlayer->iMPMax = 100;
            break;
        case JOB_ARCHER:
            strcpy_s(pPlayer->strJobName, "궁수");
            pPlayer->iAttackMin = 10;
            pPlayer->iAttackMax = 15;
            pPlayer->iArmorMin = 10;
            pPlayer->iArmorMax = 15;
            pPlayer->iHPMax = 400;
            pPlayer->iHP = 400;
            pPlayer->iMP = 200;
            pPlayer->iMPMax = 200;
            break;
        case JOB_WIZARD:
            strcpy_s(pPlayer->strJobName, "마법사");
            pPlayer->iAttackMin = 15;
            pPlayer->iAttackMax = 20;
            pPlayer->iArmorMin = 5;
            pPlayer->iArmorMax = 10;
            pPlayer->iHPMax = 300;
            pPlayer->iHP = 300;
            pPlayer->iMP = 300;
            pPlayer->iMPMax = 300;
            break;
    }
}

tMonster CreateMonster(char *pName, int iAttackMin, 
    int iAttackMax, int iArmorMin, int iArmorMax, int iHP,
    int iMP, int iLevel, int iExp, int iGoldMin, int iGoldMax)
{
    tMonster tMonster = {};
    
    strcpy_s(tMonster.strName, pName);
    tMonster.iAttackMin = iAttackMin;
    tMonster.iAttackMax = iAttackMax;
    tMonster.iArmorMin = iArmorMin;
    tMonster.iArmorMax = iArmorMax;
    tMonster.iHP = iHP;
    tMonster.iHPMax = iHP;
    tMonster.iMP = iMP;
    tMonster.iMPMax = iMP;
    tMonster.iLevel = iLevel;
    tMonster.iExp = iExp;
    tMonster.iGoldMin = iGoldMin;
    tMonster.iGoldMax = iGoldMax;

    return tMonster;
}

void SetMonster(tMonster *pMonsterArr)
{
    pMonsterArr[0] = CreateMonster("고블린", 20, 30, 2, 5,
        100, 10, 1, 1000, 500, 1500);
    pMonsterArr[1] = CreateMonster("트롤", 80, 130, 60, 90,
        2000, 100, 5, 7000, 6000, 8000);
    pMonsterArr[2] = CreateMonster("드래곤", 250, 500, 20, 400,
        30000, 20000, 10, 30000, 20000, 50000);
}

tLvUpStatus CreateLvUpStatus(int iAttackMin, int iAttackMax,
    int iArmorMin, int iArmorMax, int iHPMin, int iHPMax, 
    int iMPMin, int iMPMax)
{    
    tLvUpStatus tStatus = {};

    tStatus.iAttackMin = iAttackMin;
    tStatus.iAttackMax = iAttackMax;
    tStatus.iArmorMin = iArmorMin;
    tStatus.iArmorMax = iArmorMax;
    tStatus.iHPMin = iHPMin;
    tStatus.iHPMax = iHPMax;
    tStatus.iMPMin = iMPMin;
    tStatus.iMPMax = iMPMax;

    return tStatus;
}

int OutputStoreMenu()
{
    system("cls");
    cout << "****************** 맵 *******************" << endl;
    cout << "1. 무기상점" << endl;
    cout << "2. 방어구상점" << endl;
    cout << "3. 뒤로가기" << endl;
    cout << "메뉴를 선택하세요: ";
    int iMenu = InputInt();

    if (iMenu < SM_NONE || iMenu > SM_BACK)
        return SM_NONE;

    return iMenu;
}

int OutputStoreItemList(tInventory *pInventory, tItem *pStore, 
    int iItemCount)
{
    // 판매 목록을 보여준다
    for (int i = 0; i < iItemCount; i++) {
    cout << i + 1 <<". 이름: " << pStore[i].strName << endl;
    cout << "설명: " << pStore[i].strDesc << "\t타입: " 
        << pStore[i].strTypeName << "\n공격력: " << pStore[i].iMin << " - "
        << pStore[i].iMax << "\n구입 가격: " << pStore[i].iPrice << " Gold"
        << "\t판매 가격: " << pStore[i].iSell << " Gold" << endl << endl;       
    }
    cout << iItemCount + 1 << ". 뒤로가기" << endl;
    cout << "보유금액: " << pInventory->iGold << " Gold" << endl;
    cout << "남은공간: " << INVENTORY_MAX - pInventory->iItemCount << endl;
    cout << "구입하려는 아이템을 선택하세요(1 - 4): "; 
    int iMenu = InputInt();

    if (iMenu < 1 || iMenu > iItemCount + 1)
        return INT_MAX;

    return iMenu;
}

void BuyItem(tInventory *pInventory, tItem *pStore,
    int iItemCount, int iStoreType)
{
    while (true) {
        system("cls");
        switch (iStoreType) {
            case SM_WEAPON:
                cout << "****************** 무기상점 *******************" << endl;
                break;
            case SM_ARMOR:
                cout << "****************** 방어구상점 *******************" << endl;
                break;
        }
        int iInput = OutputStoreItemList(pInventory, pStore, iItemCount);

        if (iInput == INT_MAX) {
            cout << "잘못된 입력입니다!" << endl;
            system("pause");
            continue;
        }
        else if (iInput == iItemCount + 1)
            break;

            // 상점 판매목록 배열의 인덱스
            int iIndex = iInput - 1;
            // 인벤토리 검사
            if (pInventory->iItemCount == INVENTORY_MAX) {
                cout << "가방이 가득 찼습니다." << endl << endl;
                system("pause");
                continue;
            }
            // 돈이 부족할 경우
            else if (pInventory->iGold < pStore[iIndex].iPrice) {
                cout << "잔액이 부족합니다." << endl << endl;
                system("pause");
                continue;
            }
         
            // 처음에 iItemCount는 0으로 초기화 되어있으므로 0번 인덱스에 아이템을 추가하게 된다
            // 그리고 iItemCount는 1 증가 한다. 그래서 다음 번 추가는 1번 인덱스에 하게 된다
            pInventory->tItem[pInventory->iItemCount] = 
                pStore[iIndex];
            pInventory->iItemCount++; 
            // 골드를 차감한다
            pInventory->iGold -= pStore[iIndex].iPrice;
            cout << pStore[iIndex].strName << " 을(를) 구매하였습니다." << endl;
            cout << "보유 Gold가 " << pStore[iIndex].iPrice 
                << " Gold 만큼 차감 되었습니다." << endl << endl;
            system("pause");
    }
}

void RunStore(tInventory *pInventory, tItem *pWeapon, 
    tItem *pArmor)
{
    while (true) {
        switch(OutputStoreMenu()) {
            case SM_WEAPON:
                BuyItem(pInventory, pWeapon, STORE_WEAPON_MAX, SM_WEAPON);
                break;
            case SM_ARMOR:
                BuyItem(pInventory, pArmor, STORE_ARMOR_MAX, SM_ARMOR);
                break;
            case SM_BACK:
                return;
        }
    }
}

tItem CreateItem(char *pName, ITEM_TYPE eType, 
    int iMin, int iMax, int iPrice, int iSell, char *pDesc)
{
    tItem tItem = {};

    strcpy_s(tItem.strName, pName);
    strcpy_s(tItem.strDesc, pDesc);

    tItem.eType = eType;

    switch(eType) {
        case IT_WEAPON:
            strcpy_s(tItem.strTypeName, "무기");
            break;
        case IT_ARMOR:
            strcpy_s(tItem.strTypeName, "방어구");
            break;
    }

    tItem.iMin = iMin;
    tItem.iMax = iMax;
    tItem.iPrice = iPrice;
    tItem.iSell = iSell;

    return tItem;
}

int OutputInventory(tPlayer *pPlayer)
{
    system("cls");
    cout << "****************** 가방 *******************" << endl;

    OutputPlayer(pPlayer);

    for (int i = 0; i < pPlayer->tInventory.iItemCount; i++) {
        cout << i + 1 <<". 이름: " << pPlayer->tInventory.tItem[i].strName << endl;
        cout << "설명: " << pPlayer->tInventory.tItem[i].strDesc << "\t타입: " 
            << pPlayer->tInventory.tItem[i].strTypeName << "\n공격력: " 
            << pPlayer->tInventory.tItem[i].iMin << " - " 
            << pPlayer->tInventory.tItem[i].iMax << endl << endl;
    }

    cout << pPlayer->tInventory.iItemCount + 1 << " . 뒤로가기" << endl;
    cout << "장착할 아이템을 선택하세요: ";
    int iMenu = InputInt();

    if (iMenu < 1 || iMenu > pPlayer->tInventory.iItemCount + 1)
        return INT_MAX;

    return iMenu;
}

EQUIP ComputeEquipType(ITEM_TYPE eType)
{
    EQUIP eq;

    switch (eType) {
        case IT_WEAPON:
            eq = EQ_WEAPON;
            break;
        case IT_ARMOR:
            eq = EQ_ARMOR;
            break;
    }

    return eq;
}

void RunInventory(tPlayer *pPlayer)
{ 
    while (true) {
        int iInput = OutputInventory(pPlayer);

        if (iInput == INT_MAX)
            continue;
        else if (iInput == pPlayer->tInventory.iItemCount + 1)
            break;

        // 아이템 인덱스를 구해준다
    int idx = iInput - 1;
                    
    // 제대로 선택했을 경우 해당 아이템의 종류에 따라 장착 부위를 결정한다
    EQUIP eq = ComputeEquipType(pPlayer->tInventory.tItem[idx].eType);

    // 아이템이 장착되어 있을 경우 장착되어있는 아이템과 장착할 아이템을 교체해 주어야 한다
    // Swap 활용
    if (pPlayer->bEquip[eq] == true) {
        tItem tSwap = pPlayer->tEquip[eq];
        pPlayer->tEquip[eq] = pPlayer->tInventory.tItem[idx];
        pPlayer->tInventory.tItem[idx] = tSwap;
    }

    // 장착되어있지 않은 경우는 인벤토리 아이템에서 꺼내서 장착하고 인벤토리는 1칸 비워진다
    else {
        pPlayer->tEquip[eq] = pPlayer->tInventory.tItem[idx];

        for (int i = idx; i < pPlayer->tInventory.iItemCount - 1; i++)
            pPlayer->tInventory.tItem[i] = pPlayer->tInventory.tItem[i + 1];
                        
            pPlayer->tInventory.iItemCount--;

            // 장착했으므로 true
            pPlayer->bEquip[eq] = true;
        }

        cout << pPlayer->tEquip[eq].strName << " 을(를) 장착하였습니다." << endl;

        system("pause");
    }
}