#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>
#include <libloaderapi.h>

using namespace std;

void get_proc_id(const char* window_title, DWORD& proccess_id)
{
    GetWindowThreadProcessId(FindWindow(NULL, window_title), &proccess_id);
}

void error(const char* error_title, const char* error_message)
{
    MessageBox(NULL, error_message, error_title, NULL);
    exit(-1);
}

bool file_exists(string file_name)
{
    struct stat buffer;
    return (stat(file_name.c_str(), &buffer) == 0);
}

int main()
{
    DWORD proc_id = NULL;
    char dll_path[MAX_PATH];
    const char* dll_name = "WeAreDevs_API.dll";
    const char* window_title = "Roblox";


    if (!file_exists(dll_name))
    {
        error("file_exists", "File doesn't exist");
    }

    if (!GetFullPathName(dll_name, MAX_PATH, dll_path, nullptr))
    {
        error("GetFullPathName", "Failed to get full path");
    }

    get_proc_id(window_title, proc_id);
    if (proc_id == NULL)
    {
        error("get_proc_id", "Failed to get process ID");
    }

    HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, NULL, proc_id);
    if (!h_process)
    {
        error("OpenProcess", "Failed to open a handle to process");
    }

    void* allocated_memory = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!allocated_memory)
    {
        error("VirtualAllocEx", "Failed to allocate memory");
    }

    if (!WriteProcessMemory(h_process, allocated_memory, dll_path, MAX_PATH, nullptr))
    {
        error("WriteProcessMemory", "failed to write process memory");
    }

    HANDLE h_thread = CreateRemoteThread(h_process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocated_memory, NULL, nullptr);
    if (!h_thread)
    {
        error("CreateRemoteThread", "Failed to create remote thread");
    }

    CloseHandle(h_process);
    VirtualFreeEx(h_process, allocated_memory, NULL, MEM_RELEASE);
    MessageBox(0, "Successfully Injected!", "Success", 0);
}