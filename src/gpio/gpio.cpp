
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
#include <stdio.h>
#include <signal.h>

namespace GPIO{

volatile bool keep_running = true;

    Gpio_t::Gpio_t()
    {
        #ifdef DBG_GPIO
            std::printf("Gpio_t::Gpio_t()\n");
            std::cout<<"pin led : " <<std::to_string(RPI_V2_GPIO_P1_32)<<"\n";
            std::cout<<"pin interrupt : " <<std::to_string(RPI_V2_GPIO_P1_16)<<"\n";
        #endif        
        led_state = false ;
        init();  
        toogle();              
    }

    void Gpio_t::init(){        
        function();
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
        bcm2835_gpio_fsel( LED_OUT    , BCM2835_GPIO_FSEL_OUTP );                
        bcm2835_gpio_write(LED_OUT, LOW); 

        
        bcm2835_gpio_fsel(INTERRUPT_PIN, BCM2835_GPIO_FSEL_INPT);
            // Habilita el pull-up interno
        bcm2835_gpio_set_pud(INTERRUPT_PIN, BCM2835_GPIO_PUD_UP);
        bcm2835_gpio_fen(INTERRUPT_PIN);  // Activar detección de flanco descendente

    #ifdef DBG_GPIO
        std::cout<<"Esperando interrupción en el pin : "<< INTERRUPT_PIN <<" ... \n";
    #endif
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

    void Gpio_t::toogle() {
        bcm2835_gpio_write(LED_OUT, led_state ? HIGH : LOW);
        led_state = !led_state;
    }

    void Gpio_t::transfer(const uint8_t cmd){
     #ifdef SPI_BCM2835
             bcm2835_spi_transfer(cmd);
     #endif                
    } 

    void Gpio_t::delay_ms(const int64_t time){
         bcm2835_delay(time);
    }

    void Gpio_t::check_status(){
        while (bcm2835_gpio_lev(INTERRUPT_PIN) == HIGH) {
            bcm2835_gpio_write(LED_OUT,LOW);
            led_state=false;
        }
    }



    void Gpio_t::app(bool& flag)
    {        
    if(keep_running)
    {
       if (bcm2835_gpio_eds(INTERRUPT_PIN)) {
            // Limpia el flag de evento de detección
            bcm2835_gpio_set_eds(INTERRUPT_PIN);

            // Llama a la función que maneja la interrupción
            interrupt_handler();
            flag = false;
        }
    }
    else{
        flag = true;
    }
 /*
            if (bcm2835_gpio_lev(INTERRUPT_INPT) == LOW) {
                std::cout<<"interrupt\n";
                if (!input_interrupt) {
                    // Cambia el estado del LED
                    led_state = !led_state;
                    bcm2835_gpio_write(LED_OUT, led_state ? HIGH : LOW);

                    // Marca que el botón ha sido presionado
                    input_interrupt = true;
                    return;
                }
            } else {
                // Si el botón no está presionado, resetea la variable
                //input_interrupt = false;
                input_interrupt = true;
            }
            // Pequeña espera para evitar rebotes
            delay_ms(10);
        //}
        */
    }

    void Gpio_t::interrupt_handler() {
        printf("Interrupción recibida: Borde de bajada detectado (EDGE_FALLING).\n");
    }

    static void sig_handler(int sig) {
        keep_running = false;
    }

    void Gpio_t::function(){
        signal(SIGINT, sig_handler);
    }
}

