#include <radio/radio.hpp>
#include <mrf24/mrf24j40.hpp>
#include <string_view>
#include <memory>

namespace MRF24J40{ 

//Mrf24j_t mrf24j40_spi ;
std::unique_ptr<Mrf24j_t> mrf24j40_spi ;//= 

std::string msj_txt = {"MRF24J40 RX"};


    Radio_t::Radio_t() 
    #ifdef ENABLE_INTERRUPT_MRF24
    :   m_status          (true)
    #else
    :   m_status          (false)
    #endif
    ,   gpio            { std::make_unique<GPIO::Gpio_t>(m_status) }
    {              
        #ifdef DBG
            std::cout << "Size msj : ( "<<std::dec<<sizeof(MSJ)<<" )\n";
        #endif
        mrf24j40_spi = std::make_unique<Mrf24j_t>();

        mrf24j40_spi->init();
        mrf24j40_spi->settingsSecurity();
        mrf24j40_spi->interrupt_handler();
        mrf24j40_spi->set_pan(PAN_ID);
        // This is _our_ address

        #ifdef MACADDR16
            mrf24j40_spi->address16_write(ADDRESS); 
        #elif defined (MACADDR64)
            mrf24j40_spi->address64_write(ADDRESS_LONG);
        #endif

        // uncomment if you want to receive any packet on this channel
        mrf24j40_spi->set_promiscuous(true);
        //mrf24j40_spi->settings_mrf();
    
        // uncomment if you want to enable PA/LNA external control
        mrf24j40_spi->set_palna(true);
    
        // uncomment if you want to buffer all PHY Payload
        mrf24j40_spi->set_bufferPHY(true);

        //attachInterrupt(0, interrupt_routine, CHANGE); // interrupt 0 equivalent to pin 2(INT0) on ATmega8/168/328
        //last_time = millis();

        //Single send cmd
        //mrf24j40_spi->Transfer3bytes(0xE0C1);
            
        m_flag=true;
    }

    bool Radio_t::Run(void){         
        gpio->app(m_flag);                                            
        Start(m_flag);                
        interrupt_routine() ;  
        return m_flag; 
    }

    void Radio_t::Start(bool& flag) {

        flag = mrf24j40_spi->check_flags(&handle_rx, &handle_tx);
        const unsigned long current_time = 100000;
        if (current_time - m_last_time > m_tx_interval) {
            m_last_time = current_time;
        #ifdef MRF24_TRANSMITER_ENABLE   
            #ifdef DBG_RADIO
                #ifdef MACADDR64
                    std::cout<<"send msj 64() ... \n";
                #else
                    std::cout<<"send msj 16() ... \n";
                #endif
            #endif
            buffer_transmiter.head=HEAD; 
            buffer_transmiter.size=(~strlen(MSJ))&0xffff ;
            #ifdef DBG_RADIO
            //std::cout<<"\n strlen(MSJ) : "<<  strlen(MSJ)<<"\n";  
            #endif  
            std::strcpy(buffer_transmiter.data , MSJ);

            const char* msj = reinterpret_cast<const char* >(&buffer_transmiter);
            //  const auto* buff {reinterpret_cast<const char *>(mrf24j40_spi->get_rxinfo()->rx_data)};
            #ifdef DBG_RADIO
                std::cout<<"\n MSJ : size ( "<<  strlen(msj) <<" , "<<sizeof(msj) << " )\n" ;
                std::cout<<"\n" ;
            #endif
            const std::string pf(msj);
            #ifdef DBG_RADIO
                for(const auto& byte : pf) std::cout << byte ; 
            #endif
            std::cout<<"\n" ;         
            #ifdef USE_MRF24_TX 
                #ifdef MACADDR64
                    mrf24j40_spi->send(ADDRESS_LONG_SLAVE, msj);               
                #elif defined(MACADDR16)
                    mrf24j40_spi->send(ADDRESS_SLAVE, msj);                                
                #endif
        #endif
    //      const auto status = mrf24j40_spi->read_short(MRF_TXSTAT);//or TXNSTAT =0: Transmissionwassuccessful         
            const auto status = mrf24j40_spi->getStatusInfoTx();//mrf24j40_spi->check_ack(&handle_tx);
            if (status==0) {
                std::cout<<"\nTX ACK failed\n";
            } 
            if (status==1)  {//0 = Succeeded
                std::cout<<"\tTX ACK Ok   \n";
                #ifdef DBG_RADIO
                    std::cout<<" retries : "<<std::to_string(mrf24j40_spi->get_txinfo()->retries);
                    std::cout<<"\n";
                #endif
            }
        #endif    
        }
    }

    void Radio_t::interrupt_routine() {
        mrf24j40_spi->interrupt_handler(); // mrf24 object interrupt routine
    }

    void update(std::string_view str_view)
    {    
        const int positionAdvance{15};
        const auto* packet_data = reinterpret_cast<const char*>(str_view.data());
        std::string  PacketDataTmp (packet_data += positionAdvance);
        PacketDataTmp.resize(38);
        std::cout<<"\n";
    return ;    
    }

    void Radio_t::handle_tx() {    
        #ifdef MRF24_TRANSMITER_ENABLE
        const auto status = mrf24j40_spi->get_txinfo()->tx_ok;
             if (status) {
                 std::cout<<"\thandle_tx() : TX went ok, got ACK success ! \n";
             } else {
                std::cout<<"\n\tTX failed after \n";
                std::cout<<"retries : "<<mrf24j40_spi->get_txinfo()->retries;
                std::cout<<" \n";
             }
             #endif
        return;
    }

void Radio_t::handle_rx() {
    
    #ifdef MRF24_RECEIVER_ENABLE
    auto  size = static_cast <std::size_t>(mrf24j40_spi->get_rxinfo()->frame_length);
    std::vector<char>bufferMonitor(size);

    std::printf("received a packet ... ");    //std::cout << " \nreceived a packet ... ";
    //sprintf(bufferMonitor.data(),"0x%x\n",mrf24j40_spi->get_rxinfo()->frame_length);
    
    std::memcpy(bufferMonitor.data() , mrf24j40_spi->get_rxbuf() ,size);

    std::cout<< "tamaño del paquete : " << std::to_string(bufferMonitor.size())<<"\n";
    std::printf(bufferMonitor.data());//    std::cout << " bytes long " ;
    std::cout<< "\n\n\n";
    if(mrf24j40_spi->get_bufferPHY()){
        std::printf(" Packet data (PHY Payload) :");//  std::cout << " Packet data (PHY Payload) :";
      #ifdef DBG_PRINT_GET_INFO
      for (int i = 0; i < mrf24j40_spi->get_rxinfo()->frame_length; i++) 
      {        
          std::cout <<" "<<std::hex<< mrf24j40_spi->get_rxbuf()[i];//monitor->set(" Packet data (PHY Payload) :",files,col);
      }
      #endif
    }
        std::cout << "\n";
        
        std::printf("ASCII data (relevant data) :"); //std::cout<<"\r\nASCII data (relevant data) :\n";
        const auto recevive_data_length = mrf24j40_spi->rx_datalength();
        std::cout<<"\tdata_length : " << std::to_string(recevive_data_length) ;        
        std::printf("\n");        
        std::printf(reinterpret_cast<const char*>(mrf24j40_spi->get_rxinfo()->rx_data ));

        for (auto& byte : mrf24j40_spi->get_rxinfo()->rx_data)std::cout<<byte;        
        std::cout<<"\n";;

    #ifdef DBG_PRINT_GET_INFO 
      
        if(ADDRESS_LONG_SLAVE == add){
            std::printf("\nmac es igual\n" );
        }
        else{
            std::printf("\nmac no es igual\n") ;
        }
        std::cout<<"\ndata_receiver->mac : " << std::to_string (add )<< "\n";
        std::cout<<"buffer_receiver->head : " << packet_data_tmp->head << "\n";
        auto bs = (~packet_data_tmp->size)&0xffff;
        std::cout<<"buffer_receiver->size : " << reinterpret_cast<const int *>(bs) << "\n" ;
        std::cout<<"data_receiver->data : " << reinterpret_cast<const char *>(packet_data_tmp->data) << "\n" ;
        std::cout<<"\nbuff: \n" << buff ;
        std::printf("\r\n" );
    #endif            
        std::cout<<"LQI : " << std::to_string(mrf24j40_spi->get_rxinfo()->lqi)   <<"\n";
        std::cout<<"RSSI : "<<  std::to_string(mrf24j40_spi->get_rxinfo()->rssi) <<"\n";
    #endif
        
        
        update(reinterpret_cast<const char*>(mrf24j40_spi->get_rxinfo()->rx_data) ); //update(tempString.data());
        
        //std::printf(temperatureToString.data());
        
        msj_txt = reinterpret_cast<const char*>(mrf24j40_spi->get_rxinfo()->rx_data) ;        
                std::cout<<msj_txt <<"\n";
        return;    
    }

    Radio_t::~Radio_t() {
        #ifdef DBG
            std::cout<<"~Radio_t()\n";
        #endif
    }
}



