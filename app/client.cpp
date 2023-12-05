

#include "../lib/librlog.cpp"

using namespace std;

void calculate() {
    long long iterations = 0;

    log_remote(info, "Starting calculation for questions of meaning, of universe and other things");
    long double answer_for_all = 0;
    while (answer_for_all < 42) {
        iterations++;
        answer_for_all += 0.000000001;
        if(iterations % 10000000 == 0) {
            if(answer_for_all > 42) {
                log_remote(error, "We are reaching the end of the universe");
            }
            if(answer_for_all < 42) {
                auto message = string("The ") + to_string(answer_for_all) + string(" is not the answer!");
                log_remote(info, message);
            }
            
        }
        
    }
    auto message = string("The Answer to questions of the Meaning, of the Universe and other things is finally calculated it is equal to ") + to_string(answer_for_all) + string(" !");

    log_remote(info, message);
    sendSuccess(answer_for_all);
    log_remote(warn, "Success request sended ding peacefully. ");
}


int main(int argc, char *argv[]) {
  char *serverIp = getenv("ADDRESS");
  int port = atoi(getenv("PORT"));

  config(serverIp, port);
  connect();
  
  calculate();
  return 0;
}
