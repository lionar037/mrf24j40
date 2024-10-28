
//////////////////////////////////////////////////////////////////////////////
//     
//          filename            :   gpio.cpp
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

#include <gpio/gpio.hpp>
#include <iostream>

namespace GPIO{

    Gpio_t::Gpio_t()
    {
        #ifdef DBG_GPIO
            std::printf("Gpio_t::Gpio_t()\n");
        #endif        
        led_state = false ;
        init();
        std::cout<<"pin led : " <<std::to_string(RPI_V2_GPIO_P1_23)<<"\n";
        std::cout<<"pin interrupt : " <<std::to_string(RPI_V2_GPIO_P1_12)<<"\n";
    }

    void Gpio_t::init(){        
        #ifdef SPI_BCM2835
            if (!bcm2835_init() || !bcm2835_spi_begin()) 
        #else
            if (!bcm2835_init()) 
        #endif
            {
                //throw "Initialization failed. Are you running as root?";
                throw std::runtime_error("Initialization failed. Are you running as root?");
            }  
                                                

        #ifdef SPI_BCM2835
        bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);//for mode SPI
        bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);//for mode SPI
        bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_8);//for mode SPI
        bcm2835_spi_chipSelect(BCM2835_SPI_CS0);//for mode SPI
        #endif
        bcm2835_gpio_fsel( IN_INTERRUPT     , BCM2835_GPIO_FSEL_INPT );
        bcm2835_gpio_fsel( OUT_INTERRUPT    , BCM2835_GPIO_FSEL_OUTP );                
        // Habilita el pull-up resistor para el botón
        bcm2835_gpio_set_pud(IN_INTERRUPT, BCM2835_GPIO_PUD_UP);
        bcm2835_gpio_write(OUT_INTERRUPT, LOW); 

    }

    void Gpio_t::close(){
    #ifdef SPI_BCM2835
        bcm2835_spi_end();
    #endif
        bcm2835_close();  
    }

    Gpio_t::~Gpio_t(){
        close();
    }

    void Gpio_t::toogle(bool& led){
        if (led)
        {
            bcm2835_gpio_write(OUT_INTERRUPT, HIGH);              
        }
        else{                
            bcm2835_gpio_write(OUT_INTERRUPT, LOW); 
        }     
        led=!led;       
    }

    void Gpio_t::transfer(const uint8_t cmd){
     #ifdef SPI_BCM2835
             bcm2835_spi_transfer(cmd);
     #endif                
    } 

    void Gpio_t::delay_ms(const int64_t time){
         bcm2835_delay(time);
    }

    void Gpio_t::app(bool& input_interrupt)
    {        
        //bool button_pressed = false;
        //while (true) {
            // Lee el estado del botón (activo en bajo)
            if (bcm2835_gpio_lev(IN_INTERRUPT) == LOW) {
                if (!input_interrupt) {
                    // Cambia el estado del LED
                    led_state = !led_state;
                    bcm2835_gpio_write(OUT_INTERRUPT, led_state ? HIGH : LOW);

                    // Marca que el botón ha sido presionado
                    input_interrupt = true;
                }
            } else {
                // Si el botón no está presionado, resetea la variable
                input_interrupt = false;
            }
            // Pequeña espera para evitar rebotes
            delay_ms(10);
        //}
    }
}

