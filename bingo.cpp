/* 
1 ~ 25 까지의 숫자가 있고 이 숫자를 골고루 섞어서 5 x 5 로 출력한다.
플레이어는 숫자를 입력받는다. 1 ~ 25 사이의 숫자를 입력받아야 한다.
0을 입력하면 게임을 종료한다.

숫자를 입력받았으면 숫자목록중 입력받은 숫자를 찾아서 *로 만들어준다.
숫자를 *로 만든 후에 빙고 줄 수를 체크한다. 5 x 5 이기 때문에 가로 5줄 세로 5줄
대각선 2줄이 나올 수 있다. 빙고줄이 몇줄인지를 체크해서 화면에 보여준다.
5줄 이상일 경우 게임을 종료한다.
*/
#include <iostream>
#include <time.h>

using namespace std;

enum AI_MODE {
    AI_EASY = 1,
    AI_HARD
};

enum LINE_NUMBER
{
    LN_H1,
    LN_H2,
    LN_H3,
    LN_H4,
    LN_H5,
    LN_V1,
    LN_V2,
    LN_V3,
    LN_V4,
    LN_V5,
    LN_LT,
    LN_RT
};

void SetNumber(int *pArray);
void Shuffle(int *pArray);
AI_MODE SelectAIMode();
void OutputNumber(int *pArray, int iBingo);
bool ChangeNumber(int *pArray, int iInput);
int SelectAINumber(int *pArray, AI_MODE eMode);
int BingoCounting(int *pArray);     // 빙고 카운트
int BingoCountingH(int *pArray);    // 가로 줄 체크
int BingoCountingV(int *pArray);    // 세로 줄 체크
int BingoCountingLTD(int *pArray);  // 왼쪽 상단 가로줄 체크
int BingoCountingRTD(int *pArray);  // 오른쪽 상단 가로줄 체크

int main()
{
    srand((unsigned int)time(0));
    
    int iNumber[25] = {};
    int iAINumber[25] = {};
    int iInput;
    int iNumberIndex = 0, iZerocounter = 0;

    SetNumber(iNumber);
    SetNumber(iAINumber);

    // 숫자를 섞어준다
    Shuffle(iNumber);
    Shuffle(iAINumber);
    
    int iBingo = 0, iAIBingo = 0;

    // AI 난이도 선택
    AI_MODE eAIMode = SelectAIMode();

    while (true) {
        system("cls");

        // 5 x 5 빙고판 출력
        cout << "==================== Player =====================" << endl;
        OutputNumber(iNumber, iBingo);

        // AI 빙고판
        cout << "==================== AI ========================" << endl;

        switch (eAIMode) {
            case AI_EASY:
                cout << "AIMode : Easy" << endl;
                break;
            case AI_HARD:
                cout << "AIMode: Hard" << endl;
                break;
        }

        // AI 빙고판 출력 
        OutputNumber(iAINumber, iAIBingo);
        
        // 줄수가 5이상일 경우 게임을 종료
        if (iBingo >= 5) {
            cout << "Player's Win!" << endl;
            break;
        }
        else if (iAIBingo >= 5) {
            cout << "AI's Win!" << endl;
            break;
        }
        cout << "Enter Number(0: Exit): ";
        cin >> iInput;
    
        if (iInput == 0) {
            cout << "Game Over." << endl;
            break;
        }
        else if (iInput < 1 || iInput > 25)
            continue;

        // 중복 입력 체크 변수. 기본적으로 중복인 상태를 설정하고 true로 선언
        bool bAcc = ChangeNumber(iNumber, iInput);

        // bAcc 변수가 true일 경우 중복된 숫자를 입력하여 숫자를 *로 바꾸지 못했으므로 다시 입력
        if(bAcc)
            continue;

        // 중복이 아니라면 AI의 숫자도 찾아서 *로 바꾼다
        ChangeNumber(iAINumber, iInput);

        // AI가 숫자를 선택한다. AI모드에 맞춰서 선택되도록 함수가 구성되어 있다.
        iInput = SelectAINumber(iAINumber, eAIMode);

        // AI가 숫자를 선택했으므로 플레이어와 AI의 숫자를 *로 바꿔준다
        ChangeNumber(iNumber, iInput);
        ChangeNumber(iAINumber, iInput);

        // 빙고 줄 수를 체크하는 것은 매번 숫자를 입력할 때마다 새로 카운트 한다
        // iBingo 변수를 0으로 초기화 한다
        iBingo = BingoCounting(iNumber);
        iAIBingo = BingoCounting(iAINumber);
    }
    
    return 0;
}

void SetNumber(int *pArray)
{
    for (int i = 0; i < 25; i++) {
        pArray[i] = i + 1;
    }
}

void Shuffle(int *pArray)
{
    int idx1, idx2, iTemp;

    for (int i = 0; i < 100; i++) {
        idx1 = rand() % 25;
        idx2 = rand() % 25;

        iTemp = pArray[idx1];
        pArray[idx1] = pArray[idx2];
        pArray[idx2] = iTemp;
    }   
}

AI_MODE SelectAIMode()
{
    int iAIMode;
    while (true) {
        system("cls");
        cout << "1. Easy" << endl;
        cout << "2. Hard" << endl;
        cout << "Select AI Mode." << endl;
        cin >> iAIMode;

        if (iAIMode >= AI_EASY && iAIMode <= AI_HARD)
            break;
    }

    return (AI_MODE)iAIMode;
}

void OutputNumber(int *pArray, int iBingo)
{
     for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                if (pArray[i * 5 + j] == INT_MAX)
                    cout << "*\t";
                else
                    cout << pArray[i * 5 + j] << "\t";
            }
            cout << endl;
        }

        cout << "Bingo Line: " << iBingo << endl << endl;
}

bool ChangeNumber(int *pArray, int iInput)
{
    // 모든 숫자를 차례대로 검사해서 입력한 숫자와 같은 숫자가 있는지 검사
    for (int i = 0; i < 25; i++) {

        // 입력한 숫자와 같은 숫자가 있는 경우
        if (iInput == pArray[i]) {

            // 숫자를 *로 변경하기 위해 특수한 값인 INT_MAX로 설정
            pArray[i] = INT_MAX;
                
            // 숫자를 찾았을 경우 중복된 숫자가 아니므로 bAcc 변수를 false로 선언
            // 더이상 다른 숫자를 찾아볼 필요가 없으므로 for문 빠져나감
            return false;
        }
    }

    // 여기까지 오게 되면 return false가 동작되지 않은 것이므로 같은 숫자가 없다는 것이다
    // 즉, 중복된 숫자를 입력했기 때문에 true를 return 한다
    return true;   
}

int SelectAINumber(int *pArray, AI_MODE eMode)
{
    // 선택이 안된 목록의 배열을 만든다
    int iNoneSelect[25] = {};

    // 선택이 안된 숫자의 갯수를 저장한다.
    int iNoneSelectCount = 0;

    // AI가 선택을 한다. 선택은 AI에 따라서 달라진다
    switch (eMode)
    {
        /* 
        AI Easy 모드는 현재 AI의 숫자목록중 *로 바뀌지 않은 숫자 목록을 만들어
        그 목록 중 하나를 선택하게 한다 (랜덤)
        */
        case AI_EASY:

            // 선택 안 된 숫자 목록 만들기
            // 선택 안 된 숫자 개수는 목록을 만들 때 카운트
            iNoneSelectCount = 0;

            for (int i = 0; i < 25; i++) {

                // 현재 숫자가 *이 아닐 경우
                if (pArray[i] != INT_MAX) {
                    // *이 아닌 숫자일 경우 iNoneSelectCount를 인덱스로 활용
                    // 선택 안 된 목록에 추가할 때마다 개수를 1씩 증가시켜 선택 안 된 수
                    // 전체를 구한다. 0부터 카운트 시작이므로 인덱스 0번에 넣고 증가시켜
                    // 1개 추가 되었다고 한다
                    iNoneSelect[iNoneSelectCount] = pArray[i];
                    iNoneSelectCount++;
                }
            }

            // for 문을 빠져나오게 되면 선택 안 된 목록이 만들어지고
            // 선택 안 된 목록의 개수가 만들어진다
            // 선택 안 된 목록의 숫자 중 랜덤한 하나의 숫자를 얻어오기 위해 인덱스를
            // 랜덤하게 구해준다
            return iNoneSelect[rand() % iNoneSelectCount];
        case AI_HARD:
            // 하드모드는 현재 숫자중 빙고 가능성이 가장 높은 줄을 찾아서
            // 그 줄에 있는 숫자중 하나를 *로 만든다(별이 가장 많은 줄. 단 별 5개 제외)
            int iLine = 0;
            int iStarCount = 0; // 별의 숫자 저장
            int iSaveCount = 0; // 별이 가장 많은 줄 저장

            // 가로 줄 중에 가장 별이 많은 줄을 찾는다
            for (int i = 0; i < 5; i++) {
                iStarCount = 0;
                for (int j = 0; j < 5; j++) {
                    if (pArray[i * 5 + j] == INT_MAX)
                        iStarCount++;
                }
                // 별이 5개 미만이어야 빙고 줄이 아니고 기존에 가장 많은 별을 가진 줄보다
                // 별이 더 많다면, 별이 더 많은 줄로 교체해준다
                if (iStarCount < 5 && iSaveCount < iStarCount) {
                    // 가로 줄 중에 가장 별이 많은 줄을 체크하는 곳
                    // 가로 줄은 0 ~ 4 로 의미 부여
                    iLine = i;  // 가로 줄 번호
                    iSaveCount = iStarCount;
                }
            }
            // 가로 줄 중에 가장 별이 많은 줄을 구했다면 이 값과 세로 줄을 비교하여
            // 별이 가장 많은 줄을 구한다
            for (int i = 0; i < 5; i++) {
                iStarCount = 0;
                for (int j = 0; j < 5; j++) {
                    if (pArray[j * 5 + i] == INT_MAX)
                        iStarCount++;
                }

                if (iStarCount < 5 && iSaveCount < iStarCount) {
                    // 세로 줄은 5 ~ 9 로 의미 부여
                    iLine = i + 5;
                    iSaveCount = iStarCount;
                }
            }

            // 왼쪽 -> 오른쪽 대각선 체크
            iStarCount = 0;
            for (int i = 0; i < 25; i += 6) {
                if (pArray[i] == INT_MAX)
                    iStarCount++;
            }

            if (iStarCount < 5 && iSaveCount < iStarCount) {
                iLine = LN_LT;
                iSaveCount = iStarCount;
            }

            // 오른쪽 -> 왼쪽 대각선 체크
            iStarCount = 0;
            for (int i = 4; i <= 20; i += 4) {
                if (pArray[i] == INT_MAX)
                    iStarCount++;
            }

            if (iStarCount < 5 && iSaveCount < iStarCount) {
                iLine = LN_RT;
                iSaveCount = iStarCount;
            }

            // 모든 라인을 조사했다면 iLine에 가장 빙고 가능성이 높은 줄 번호가 저장됨
            // 그 줄에 있는 *이 아닌 숫자 중에 하나를 선택한다
            // 가로 줄일 경우
            if (iLine <= LN_H5) {

                // 가로 줄일 경우 iLine이 0 ~ 4 사이의 값이다
                for (int i = 0; i < 5; i++) {

                    // 현재 줄에서 *이 아닌 숫자를 찾아냄
                    if (pArray[iLine * 5 + i] != INT_MAX) {
                        return pArray[iLine * 5 + i];
                    }
                }
            }
            else if (iLine <=LN_V5) {

                // 세로 줄일 경우 iLine이 5 ~ 9 사이의 값이다
                for (int i = 0; i < 5; i++) {
                    if (pArray[i * 5 + (iLine - 5)] != INT_MAX) {
                        return pArray[i * 5 +(iLine - 5)];
                    }
                }
            }
            else if (iLine == LN_LT) {
                for (int i = 0; i < 25; i += 6) {
                    if (pArray[i] != INT_MAX) {
                        return pArray[i];
                    }
                }
            }
            else if (iLine == LN_RT) {
                for (int i = 4; i <= 20; i += 4) {
                    if (pArray[i] != INT_MAX) {
                        return pArray[i];
                    }
                }
            }
            break;
    }

    return -1;  // 잘못된 경우
}

int BingoCounting(int *pArray)
{
    int iBingo = 0;

    // 가로줄 체크
    iBingo += BingoCountingH(pArray);
    
    // 세로줄 체크
    iBingo += BingoCountingV(pArray);

    // 왼쪽 상단 대각선 체크
    iBingo += BingoCountingLTD(pArray);

    // 오른족 상단 대각선 체크
    iBingo += BingoCountingRTD(pArray);

    return iBingo;

} 
int BingoCountingH(int *pArray)
{
    // 가로 줄 수 체크
    int iStar1 = 0, iBingo = 0;
    for (int i = 0; i < 5; i++) {

        // 다음 줄로 넘어 가기 전에 0으로 초기화
        iStar1 = 0;

        for (int j = 0; j < 5; j++) {
            // 가로 별 수
            if (pArray[i * 5 + j] == INT_MAX)
                iStar1++;
        }

            // j for문 빠져나오면 가로 줄의 별은 몇 개인지 iStar1 변수에 들어감
            // iStar1이 5이면 한 줄이 모두 별이라는 의미이므로 빙고 줄 카운트가 올라간다
        if (iStar1 == 5)
            iBingo++;
    }

    return iBingo;
}    
int BingoCountingV(int *pArray)
{
    // 세로 줄 수 체크
    int iStar1 = 0, iBingo = 0;
    for (int i = 0; i < 5; i++) {

        // 다음 줄로 넘어 가기 전에 0으로 초기화
        iStar1 = 0;

        for (int j = 0; j < 5; j++) {

            // 세로 별 수
            if (pArray[j * 5 + i] == INT_MAX)
                iStar1++;
        }

        // j for문 빠져나오면 가로 줄의 별은 몇 개인지 iStar1 변수에 들어감
        // iStar1이 5이면 한 줄이 모두 별이라는 의미이므로 빙고 줄 카운트가 올라간다
        if (iStar1 == 5)
            iBingo++;
    }

    return iBingo;
}

int BingoCountingLTD(int *pArray)
{
    // 왼쪽 상단에서 시작해서 오른쪽 하단으로 가는 대각선 체크
    int iStar1 = 0, iBingo = 0;

    for (int i = 0; i < 25; i += 6) {
        if (pArray[i] == INT_MAX)
            iStar1++;
    }
    if (iStar1 == 5)
        iBingo++;
    
    return iBingo;
}
int BingoCountingRTD(int *pArray)
{
    // 오른쪽 상단에서 시작해서 왼쪽 하단으로 가는 대각선 체크
    int iStar1 = 0, iBingo = 0;
    for (int  i = 4; i < 21; i += 4) {
        if (pArray[i] == INT_MAX)
            iStar1++;
    }
    if (iStar1 == 5)
        iBingo++;
    
    return iBingo;
}