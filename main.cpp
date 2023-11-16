#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

void fetchRepository(const string& repoPath){
    cout << "ID процесу: " << getpid() << endl;

    filesystem::current_path(repoPath);

    string command = "git fetch origin";
    int result = system(command.c_str());

    if (result == 0) {
        cout << "Репозиторій в папці " << repoPath << " успішно оновлений." << endl;

    } else {
        cout << "Не вдалось оновити репозиторій " << repoPath << endl;
    }

}

void processFolder(const string& folderPath, vector<string>& repositories) {

    for (const auto& entry : filesystem::directory_iterator(folderPath)) {

        if (entry.is_directory() && filesystem::exists(entry.path() / ".git")) {
            repositories.push_back(entry.path().string());
        }
    }
}

int main() {
    setlocale(LC_ALL, "ua");

    string folderPath = "/Users/artemkorolcuk/Documents/repo";

    if(!filesystem::exists(folderPath)){
        cerr << "Помилка: неможливо відкрити папку " << folderPath << endl;
        return 1;
    }

    vector<string> repositories;
    processFolder(folderPath, repositories);

    cout << "Список репозиторіїв:\n";
    for (const auto& repo : repositories) {
        cout << repo << endl;
    }

    for (const auto& repo : repositories) {
        pid_t pid = fork();

        if (pid == 0) {
            fetchRepository(repo);
            exit(0);
        } else if (pid < 0) {
            cerr << "Помилка при створенні процесу" << endl;
            return 1;
        }
    }

    int status;
    while (wait(&status) > 0);
    return 0;
}
