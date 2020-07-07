## SimpleUE4
Simple 예제의 서버와 통신을 주고 받는 UE4 클라이언트 예제입니다.

### 준비사항
1. Visual Studio 2019 버전이 설치되어있으셔야 됩니다.
2. Epic Launcher로 UE4 Engine이 설치되어있으셔야 됩니다. ( SimpleUE4 제작에 사용한 UE4 Engine의 버전은 4.25입니다. SimpleUE4.uproject 파일에 기재되어 있습니다. )

### 실행방법
1. 프라우드넷 설치 경로 상 util 폴더의 PIDL.exe로 Simple 폴더의 PIDL 파일들을 PIDL 컴파일해줍니다.
2. SimpleUE4 폴더의 SimpleUE4.uproject 파일을 더블클릭합니다. ( PIDL 파일들을 먼저 컴파일해주지 않으면 열리지 않습니다. )
3. ".... Would you like to rebuild them now?"라는 MessageBox가 뜨면, '예(Y)'를 누릅니다.
4. UE4 Editor가 SimpleUE4에 대해서 구동이 완료되었지만, "라이팅을 다시 빌드해야 됩니다."라고 뷰포트에 빨간 글씨로 뜨신다면, '빌드' 버튼을 펼치시고 '라이팅만 빌드'를 누릅니다.
5. 라이팅 빌드가 완료되었다면, SimpleUE4.sln 파일을 Visual Studio로 엽니다.
6. SimpleUE4.Build.cs 파일을 엽니다.
7. 하기와 같이 적혀있는 헤더 파일 참조 절대 경로를 프라우드넷 설치 폴더 상의 Include 폴더 절대 경로로 바꿉니다.
```PublicIncludePaths.Add(@"E:\engine\dev003_3478\R2\ProudNet\include);```
8. 하기와 같이 적혀있는 ProudNetClient.lib 참조 절대 경로를 프라우드넷 설치 폴더 상의 Release버전 64비트용 ProudNetClient.lib가 있는 절대 경로로 바꿉니다.
```PublicIncludePaths.Add(@"E:\engine\dev003_3478\R2\ProudNet\include);```
9. 프라우드넷 설치 경로의 Bin 폴더 안에 있는 CMake 설치 파일로 CMake가 설치되어져 있지 않으시다면 CMake를 설치합니다.
10. CMake 설치가 완료되셨다면, Simple 폴더에서 cmd 창을 띄웁니다.
11. cmd에서 하기와 같은 명령으로 Visual Studio project 파일로 솔루션 파일을 생성합니다.
```cmake -G "Visual Studio 16 2019"```
설치하신 CMake가 지원하는 Visual Studio 버전을 확인하시려면 하기의 명령을 cmd에서 입력해보시면 됩니다.
```cmake -G```
12. 생성된 sln 파일을 여신 뒤, Common 프로젝트를 빌드합니다.
13. 하기와 같이 적혀있는 SimpleCommon.lib 참조 절대 경로를 방금 전에 빌드하셔서 생성된 SimpleCommon.lib가 있는 절대 경로로 바꿉니다.
```PublicIncludePaths.Add(@"E:\engine\dev003_3478\R2\ProudNet\include);```
14. 이제 UE4 Editor 창에서 '컴파일'을 한번 눌러줘서 C++ 코드들이 컴파일되게 합니다.
15. 컴파일 완료 메세지가 뷰포트 우측 하단에 뜨면, Client인 SimpleUE4는 구동 준비가 끝난 것입니다.
16. SimpleUE4 클라이언트를 구동시키기 전에 메세지를 주고 받을 Simple 서버를 실행시켜둬야 합니다. 11번에서 생성된 sln 파일을 여신 뒤, Server를 빌드하여 실행시킵니다. 
17. 이제 SimpleUE4로 실행중인 UE4 Editor에서 '플레이'버튼을 눌러서 Editor 내에서 SimpleUE4를 실행시켜봅니다.
18. 실행 직후 SimpleUE4는 서버와 Chat & ShowChat 메세지를 주고 받습니다.
19. 이제 콘솔 버전의 Simple 클라이언트도 빌드해서 띄웁니다.
20. 서버 콘솔창에서 1을 입력해서 접속된 두 개의 클라이언트를 하나의 p2P그룹으로 묶습니다.
21. 클라이언트 한쪽에서 P2P 메세지를 보내봅니다.

### 향후 수정해야 될 사항들
1. 현재 SimpleUE4는 매 프레임마다 Tick 함수를 통해서 C++ 코드의 전역 문자열을 busy polling하고 있습니다. C++ 코드에서 블루프린트의 노드를 실행시킬 수 있는 방법을 알게되면 수정해야 됩니다.
2. 현재 SimipleUE4 화면에 보이는 multi-line 문자열은 UE4가 제공하는 ListView를 이용하고 있지 않습니다. UE4의 ListView를 이용할 수 있도록 수정해야 됩니다.

