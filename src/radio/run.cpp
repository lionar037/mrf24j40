
#include <app/run.hpp>
#include <radio/radio.hpp>
#include <config/config.hpp>
#include <mrf24/mrf24j40.hpp>
#include <iostream>
#include <thread>
#include <vector>

//#if (defined(__SIZEOF_POINTER__) && (__SIZEOF_POINTER__ == 4))

 
namespace MRF24J40{
    extern std::string msj_txt;
}


namespace RUN{
    extern MRF24J40::Mrf24j_t mrf24j40_spi ;    
    
void Run_t::start()
{
        [[gnu::unused]]  bool flag{true};
        system("clear"); 
    
    try{
            auto zigbee { std::make_unique<MRF24J40::Radio_t>()};        // Inicializar hilos y ejecutar las clases en paralelo
                 

            //std::thread thread2(&DEVICES::Msj_t::Start, msj.get());
            //Esperar a que todos los hilos terminen
                                         
            //std::thread thread1([mrf = std::move(mrf)]() {});            
            //thread1.join();
            //thread2.join();            
            
            //ip->GetHostname(MRF24J40::msj_txt);
            while(true)
            
            {                                
                flag= zigbee->Run();     
                #ifdef USE_MRF24_RX
                if(flag==true){                                                        

                }
                #endif                                
            }

                
        }//end try
        catch(...){
                    std::cerr<<"\nerror :(\n";
        }
    }
}
