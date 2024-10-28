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
    constexpr int IN_INTERRUPT = RPI_V2_GPIO_P1_23;//RPI_V2_GPIO_P1_23
    constexpr int OUT_INTERRUPT = RPI_V2_GPIO_P1_12;//RPI_V2_GPIO_P1_12
#else
    //constexpr int IN_INTERRUPT = 535;//restarle 512
    //constexpr int OUT_INTERRUPT = 524;//restarle 512
    constexpr int IN_INTERRUPT = RPI_V2_GPIO_P1_23;
    constexpr int OUT_INTERRUPT = RPI_V2_GPIO_P1_12;
#endif

    struct Gpio_t{
        public:
            explicit    Gpio_t();
            ~Gpio_t();//=default noexcept;        
            void        app(bool& ) ;//;{return true;}
            void        transfer(uint8_t );
            void        delay(const int64_t);
            void        toogle(bool&);
        protected:
            void init();
            void close();
        private :
            static bool led_state;
    };
}
