#pragma once

#include <gpio/gpio.hpp>
#include <config/config.hpp>
#include <radio/data.hpp>

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include <iomanip>
#include <functional>

#define POSITIOM_INIT_PRINTS 4

#ifdef USE_MAC_ADDRESS_LONG 
    #define MACADDR64
#elif defined (USE_MAC_ADDRESS_SHORT)
    #define MACADDR16
#endif

//namespace DATA{
//    struct packet_tx;
//}

namespace MRF24J40{

   struct Radio_t
   {
        public:
            explicit            Radio_t();
                                ~Radio_t();
            void                Start(bool&);
            void                interrupt_routine();
            bool                Run(void);
            friend void         update();  

            static void         handle_tx();
            static void         handle_rx();

            void                funcion(std::function<void(uint8_t*)> rx);

        private :
            unsigned long       m_last_time{0};
            unsigned long       m_tx_interval{1000}; 
            bool                m_status{false};
            bool                m_flag {false};            
                                            
            
        #ifdef ENABLE_INTERRUPT_MRF24 // rx
            struct DATA::packet_rx             buffer_receiver{};        
        #endif             
            struct DATA::packet_tx                  buffer_transmiter{};
        std::unique_ptr <GPIO::Gpio_t> gpio{};    
    };


}//end MRF24J40
