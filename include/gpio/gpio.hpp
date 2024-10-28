//////////////////////////////////////////////////////////////////////////////
//     
//          filename            :   gpio.h
//          License             :   GNU 
//          Author              :   Lio
//          Change History      :
//          Processor           :   ARM
//          Hardware            :		
//          Complier            :   ARM
//          Company             :
//          Dependencies        :
//          Description         :
//          brief               :	
//
//////////////////////////////////////////////////////////////////////////////

#pragma once
#include <bcm2835.h>
#include <cstdint>


namespace GPIO{

#if defined(__SIZEOF_POINTER__) && (__SIZEOF_POINTER__ == 4)
    // Si es de 32 bits
    constexpr int IN_INTERRUPT = RPI_V2_GPIO_P1_23; //RPI_V2_GPIO_P1_23//pin fisico 16
    constexpr int LED_OUT = RPI_V2_GPIO_P1_12;      //RPI_V2_GPIO_P1_12//pin fisico 32
#else
    constexpr int IN_INTERRUPT = 535;//restarle 512
    constexpr int LED_OUT = 524;//restarle 512
#endif

    struct Gpio_t{
        public:
            Gpio_t      ();
            ~Gpio_t     ();
            void        app(bool& ) ;
            void        transfer(const uint8_t );
            void        delay_ms(const int64_t);
            void        toogle();
        protected:
            void        init();
            void        close();
            void        check_status();
            
            bool        interrupt_handler();
            //void        sig_handler(int sig);
            void        function();

        private :
            inline static bool led_state;
            static constexpr int INTERRUPT_PIN = RPI_V2_GPIO_P1_16;
            static constexpr int LED_OUT = RPI_V2_GPIO_P1_32;
    };
}
