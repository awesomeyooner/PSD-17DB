#include "core.hpp"

#include "gpio.h"
#include "i2c.h"
#include "can.h"
#include "stm32f4xx_hal_can.h"

#include "EmbeddedLib/devices/gpio_device.hpp"

#include "WireLib/communication/protocols/serial_interface.hpp"
#include "WireLib/communication/protocols/i2c/i2c_interface.hpp"
#include "WireLib/communication/wire_manager.hpp"

#include "ActionLib/ActionManager.hpp"

#include <vector>


using namespace status_utils;
using namespace std;


GPIODevice led = GPIODevice(GPIOC, GPIO_PIN_1);

CAN_TxHeaderTypeDef   TxHeader;
uint8_t               TxData[8];
uint32_t              TxMailbox;

CAN_RxHeaderTypeDef   RxHeader;
uint8_t               RxData[8];


void init()
{
    CAN_FilterTypeDef filter;

    filter.FilterActivation = CAN_FILTER_ENABLE;

    filter.FilterBank = 0;
    filter.SlaveStartFilterBank = 14;

    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;

    filter.FilterIdHigh = 10 << 5;
    // filter.FilterIdHigh = 0;
    filter.FilterIdLow = 0;

    // filter.FilterMaskIdHigh = 0;
    filter.FilterMaskIdHigh = 0x7FF << 5;  // Mask All ID Bits, so only 1 ID is allowed
    filter.FilterMaskIdLow = 0; // Ignore all 0 bits

    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;

    HAL_CAN_ConfigFilter(&hcan1, &filter);

    HAL_CAN_Start(&hcan1);

    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

    // Using Standard ID (not extended)
    TxHeader.IDE = CAN_ID_STD;
    
    // ID is 10
    TxHeader.StdId = 10;
    
    // Indicate we are sending data rather than retrieve (CAN_RTR_REMOTE)
    TxHeader.RTR = CAN_RTR_DATA;

    // Length of data (# of bytes)
    TxHeader.DLC = 8;

    TxHeader.TransmitGlobalTime = DISABLE;

    for(int i = 0; i < 8; i++)
    {
        TxData[i] = i;
    }

    ActionManager::add(
        Action(0.5).link_callback(
            [](double, double) -> StatusedValue<bool>
            {
                // led.toggle();

                return StatusedValue<bool>(false, StatusCode::OK);
            }
        )
    );

} // end of "init()"


void update()
{
    ActionManager::update();
    
} // end of "update()"


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);

    ActionManager::add(
        Action::run_once(
            [](double)
            {
                Serial.println("Received Data!");

                for(int i = 0; i < 8; i++)
                {
                    Serial.println(RxData[i]);

                }
            }
        )
    );
  
    HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);

  // Get ID of sender using RxHeader.StdId

}