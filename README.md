# MainBoardCR30

## Hardware Debug Tests
This branch contains various tests for the hardware on Main Board.

Due to time constraints, the tests were written by Copilot, and many (if not all) of them **are likely to fail**.  Regardless, this will hopefully be better than nothing, and it should give us a starting point to get Main Board operational.

### Blue LED Blink Codes (Failed Tests)
The board will ideally turn the blue LED on when it starts running tests.

If all tests are successful, the LED should slowly blink on and off at a constant rate, with a period of 1 second (500 ms on, 500 ms off).

Otherwise, when the tests complete, the board should start cycling through all the failed tests (100 ms on, 100 ms off, 300 ms delay between codes).

| # of Blinks | Failed Test           | Use Case                    | Importance |
| :---------: | --------------------- | --------------------------- | ---------- |
| 1           | ADC                   | Hardware Sensors            | **HIGH**   |
| 2           | CAN                   | Inverter Communication, IMU | **HIGH**   |
| 3           | I2C                   | GPS                         | Medium     |
| 4           | LoRa                  | Wireless Communication      | Medium     |
| 5           | SD Card               | Data Logging                | **HIGH**   |
| 6           | DMA                   | Automatic Memory Transfer   | Low        |
| 7           | UART3                 | Dash Display                | Medium     |