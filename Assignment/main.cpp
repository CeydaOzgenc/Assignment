#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <fstream>

const int teamCount = 400;
const int teamRunnersCount = 4;

struct Runner {
    std::string id;
    int startPosition;
    int position;
    int speed;
    double timer;
    double finishTime;
    std::mutex mutex;
};
struct Team {
    int teamId;
    Runner runnerId[teamRunnersCount];
    int ranking;
    int momentRunner;
    double finishTime;
    std::mutex mutex;
};

void RunnerRun(Runner* runner) {
    std::lock_guard<std::mutex> lock(runner->mutex);
    runner->position += runner->speed;
    runner->timer += 1;
}

int main() {

    Team teams[teamCount];
    int input;
    std::vector<int> write;
    std::vector<std::thread> threads;
    threads.reserve(teamCount * teamRunnersCount);
    // Takim ve kosucu bilgilerinin olusturulmasi
    for (int i = 0; i < teamCount; ++i) {
        teams[i].teamId=i+1;
        teams[i].momentRunner=0;
        teams[i].finishTime=0;
        for(int j=0; j<teamRunnersCount; ++j){
            teams[i].runnerId[j].id=std::to_string(i+1) + std::to_string(j+1);
            teams[i].runnerId[j].startPosition=j*100;
            teams[i].runnerId[j].position=j*100;
            teams[i].runnerId[j].speed=rand() % 5 + 1;
            teams[i].runnerId[j].timer=1;
            teams[i].runnerId[j].finishTime=0;
        }
    }

    // Oyuncularin konum ve hizlarinin yazdirilmasi
    for (int i = 0; i < teamCount; ++i) {
        std::cout << std::endl<< "--- " << teams[i].teamId << ". Takim ---" << std::endl<< std::endl;
        for(int j=0; j<teamRunnersCount; ++j){
            std::cout << teams[i].runnerId[j].startPosition << ". metrede | " << teams[i].runnerId[j].speed << " m/s" << std::endl << std::endl;
        }
    }

    //Hangi takimlarin bilgisinin cikti olarak verilecegini kullanicidan alinmasi
    std::cout << "Bilgisini istediginiz takimlari alt alta yaziniz(Not: 0 yazarsaniz islem durdurulacak ve istediginiz takimlarin program sonunda ciktisi verilecek.)" << std::endl;
    std::cin >> input;
    write.push_back(input);
    while(input!=0){
        std::cout << "Bilgisini istediginiz takimlari alt alta yaziniz" << std::endl;
        std::cin >> input;
        write.push_back(input);
    }
    write.pop_back();

    // Kosu yarisi
    int finishcount=0;
    int ranking[teamCount];
    while(finishcount!=teamCount){
        for (int i = 0; i < teamCount; ++i) {
            int j;
            if(teams[i].momentRunner==teamRunnersCount){
                j=teams[i].momentRunner-1;
            }else{
                j=teams[i].momentRunner;
            }
            if(teams[i].runnerId[j].position<400){
                if(teams[i].runnerId[j].position==teams[i].runnerId[j].startPosition+100 || (teams[i].runnerId[j].position==teams[i].runnerId[j].startPosition+99 && teams[i].runnerId[j].speed==3)){
                    if(teams[i].runnerId[j].position==teams[i].runnerId[j].startPosition+99){
                        teams[i].runnerId[j].position+=1;
                        teams[i].runnerId[j].timer+=0.33;
                        if(teams[i].momentRunner==3){goto end;}
                    }
                    //Bayrak degisimi
                    teams[i].momentRunner+=1;
                    teams[i].runnerId[j].finishTime=teams[i].runnerId[j].timer-1;
                    teams[i].finishTime+=teams[i].runnerId[j].finishTime;
                    j=teams[i].momentRunner;
                }
                threads.emplace_back([&teams, i, j]() {
                    RunnerRun(&teams[i].runnerId[j]);
                });
            }else if(teams[i].momentRunner!=teamRunnersCount){
                //Bitis cizgisine ilk kosucu ulastiginda bilgileirn yazmasi
                if(finishcount==0){
                    std::cout << std::endl << "Bitis cizgisine ilk ulasan: "<< teams[i].teamId << ". Takim - ";
                    std::cout << "Kosucu konumlari: 1. "<< teams[i].runnerId[0].position << "m 2. "<< teams[i].runnerId[1].position;
                    std::cout << "m 3. "<< teams[i].runnerId[2].position<< "m 4. "<< teams[i].runnerId[3].position<<"m"<< std::endl;
                    teams[i].runnerId[0].position;
                }
                teams[i].runnerId[j].finishTime=teams[i].runnerId[j].timer-1;
                teams[i].finishTime+=teams[i].runnerId[j].finishTime;
                teams[i].momentRunner=teamRunnersCount;
                ranking[finishcount]=teams[i].teamId;
                finishcount+=1;
                teams[i].ranking=finishcount;
            }
            end:;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    for (auto& thread : threads) {
        thread.join();
    }

    // Yarısin takim siralamasinin yazdirilmasi
    std::cout << std::endl<< "--- Takim Siralamasi ---" << std::endl<< std::endl;
    for (int i = 0; i < teamCount; ++i) {
        std::cout << i+1 << ". => " << ranking[i] << " Takim" << std::endl;
    }

    //Secilen takimlerin ciktisi
    std::ofstream file("info.txt");
    if (file.is_open()) {
        for (const auto& element : write) {
            file << teams[element].teamId<< ". Takim" << std::endl;
            file << "Bayrak yarisi bitiris süresi: " << teams[element].finishTime <<" s"<< std::endl;
            file << "Sıralaması: " << teams[element].ranking << std::endl;
            file << "1.Koşucu hızı: " << teams[element].runnerId[0].speed<< " m/s parkur bitirme hızı: "<< teams[element].runnerId[0].finishTime<<" s" << std::endl;
            file << "2.Koşucu hızı: " << teams[element].runnerId[1].speed<< " m/s parkur bitirme hızı: "<< teams[element].runnerId[1].finishTime<<" s" << std::endl;
            file << "3.Koşucu hızı: " << teams[element].runnerId[2].speed<< " m/s parkur bitirme hızı: "<< teams[element].runnerId[2].finishTime<<" s" << std::endl;
            file << "4.Koşucu hızı: " << teams[element].runnerId[3].speed<< " m/s parkur bitirme hızı: "<< teams[element].runnerId[3].finishTime<<" s" << std::endl;
            file << std::endl;
        }

        file.close();
        std::cout << "Dosyaya yazma tamamlandi." << std::endl;
    } else {
        std::cout << "Dosya olusturulamadi." << std::endl;
    }

    return 0;
}
