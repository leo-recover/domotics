/**
 * @file nRF24L01.c
 *
 * @date 22/09/2014 18:30:05
 * @authors Stefan Engelke, Leonardo Ricupero
 */ 

#include <radio.h>

/**
 * \brief Setup the RF24 module
 * 
 * Edit this function in order to change the initial configuration
 * of the radio module
 * 
 * \return void
 */
void Radio__Initialize(void)
{
	uint8_t val[5];
	
	// EN_AA - (enable auto-acknowledgments)
	// Transmitter gets automatic response from receiver in case of successful transmission
	// It only works if the TX module has the same RF_Address on its channel. ex: RX_ADDR_P0 = TX_ADDR
	val[0] = 0x03;	
	RF24ReadWrite(W, EN_AA, val, 1);
	
	// SETUP_RETR (the setup for "EN_AA")
	// 0b0010 00011 "2" sets it up to 750uS delay between every retry (at least 500us at 250kbps and if payload >5bytes in 1Mbps, and if payload >15byte in 2Mbps) "F" is number of retries (1-15, now 15)
	val[0] = 0x2F;
	RF24ReadWrite(W, SETUP_RETR, val, 1);
	
	// Choose the number of the enabled RX data pipe (0-5)
	val[0] = 0x03;
	RF24ReadWrite(W, EN_RXADDR, val, 1); // enable data pipe 0 and 1
	
	// RF_Address width setup: how many bytes is the receiver address
	val[0] = 0x03;
	RF24ReadWrite(W, SETUP_AW, val, 1); // 0b0000 00011 5byte RF_Adress
	
	// RF channel setup - choose frequency 2.401 - 2.527 GHz, 1 MHz/step
	val[0] = 0x4C;
	RF24ReadWrite(W, RF_CH, val, 1); // 0b1101 0100 = 2,476 GHz (same on TX and RX)
	
	//RF setup - choose power mode and data speed
	val[0] = 0x07;
	RF24ReadWrite(W, RF_SETUP, val, 1); //0b0000 0111 bit 3="0" 1Mbps=longer range, bit 2-1 power mode ("11" = 0dB)
	
	// RX RF_Adress width setup 5 byte - set receiver address (set RX_ADDR_P0 = TX_ADDR if EN_AA is enabled)
	// Address = 0x01 00 01 00 01
	/*
	val[0] = '1';
	val[1] = 'N';
	val[2] = 'o';
	val[3] = 'd';
	val[4] = 'e';
	*/
	// TO-DO: Make it configurable
	val[0] = 0x01;
	val[1] = 0x00;
	val[2] = 0x01;
	val[3] = 0x00;
	val[4] = 0x01;
	
	// Since we chose pipe 0 and 1 on EN_RXADDR, we give this address to those channels.
	// P1 is the primary receiver address.
	// It is possible to assign different addresses to different channels (if they are enabled in EN_RXADDR) in order
	// to listen on several different transmitters
	RF24ReadWrite(W, RX_ADDR_P1, val, 5);
	
	// TX RF_Adress setup 5 byte - set TX address
	// Not used in a receiver but can be set anyway. Equal to Rx address
	RF24ReadWrite(W, TX_ADDR, val, 5);
	
	// Enable dynamic payload length and payload with ack packet
	val[0] = 0x06;
	RF24ReadWrite(W, FEATURE, val, 1); // 0b0000 0110: EN_DPL = 1, EN_ACK_PAY = 1
	
	// Choose the pipes with dynamic payload length. 0 and 1.
	val[0] = 0x03;
	RF24ReadWrite(W, DYNPD, val, 1);
	
	/*
	// Payload width setup - how many bytes to send per transmission (1-32)
	val[0] = 32;	// 32 bytes per package (same on RX and TX)
	RF24ReadWrite(W, RX_PW_P0, val, 1);
	*/
	
	// CONFIG reg setup - boot up the nRF24L01 and choose if it is a transmitter or a receiver
	// 0b0001 1111 - bit 0="0": transmitter bit 0="1":Receiver, bit 1="1":power up,  
	// bit 4="1": mask_Max_RT i.e. IRQ-interrupt is not triggered if transmission failed
	val[0] = 0x1F;  
	RF24ReadWrite(W, CONFIG, val, 1);
	
	//device need 1.5ms to reach standby mode
	_delay_ms(100);
}

/**
 * \brief Read a register value from nRF24L01
 *
 * \param reg
 *
 * \return uint8_t
 */
uint8_t RF24GetReg(uint8_t reg)
{
	_delay_us(10);
	PORTB &= ~(1 << PORTB2);	// CSN low
	_delay_us(10);
	SPIWriteByte(R_REGISTER + reg);	// R_REGISTER = set the RF24 to reading mode, "reg" is the register that will be read back
	_delay_us(10);
	reg = SPIWriteByte(NOP);	// Send a NOP (dummy byte) in order to read the first byte of the register "reg"
	_delay_us(10);
	PORTB |= (1 << PORTB2); //CSN IR_High
	return reg;	// Return the read register
}

/**
 * \brief Write to or Read from nRF24L01
 *
 * \param ReadWrite		Specifies if we want to read ("R") or write ("W") to the register
 * \param reg			The register to read or write
 * \param val			array to read or write
 * \param nVal			size of the array
 *
 * \return uint8_t *	array of bytes read
 */
uint8_t *RF24ReadWrite(uint8_t ReadWrite, uint8_t reg, uint8_t *val, uint8_t nVal)
{
	cli();	//disable global interrupt

	//! If "W" we want to write to nRF24. No need to "R" mode because R=0x00
	if (ReadWrite == W)
	{
		reg = W_REGISTER + reg;	//ex: reg = EN_AA: 0b0010 0000 + 0b0000 0001 = 0b0010 0001
	}

	//! A static uint8_t is needed to return an array
	static uint8_t ret[DATA_LEN];

	//! Makes sure we wait a bit
	_delay_us(10);
	PORTB &= ~(1 << PORTB2);	// CSN low
	_delay_us(10);
	SPIWriteByte(reg);	// set to write or read mode the register "reg"
	_delay_us(10);

	int i;
	for(i=0; i < nVal; i++)
	{
		// We want to read a register
		if (ReadWrite == R && reg != W_TX_PAYLOAD)
		{
			// Send dummy bytes to read the data
			ret[i] = SPIWriteByte(NOP);
			_delay_us(10);
		}
		else
		{
			SPIWriteByte(val[i]);
			_delay_us(10);
		}
	}
	PORTB |= (1 << PORTB2);	//CSN IR_High

	sei(); //enable global interrupt

	return ret;
}



/**
 * \brief Transmit a data packet with the radio module
 * 
 * \param WBuff
 * 
 * \return void
 */
void RF24TransmitPayload(uint8_t *WBuff)
{
	// Sends 0xE1 to flush the register from old data
	RF24ReadWrite(R, FLUSH_TX, WBuff, 0);
	// Sends data in WBuff to the module
	// Note that FLUSH_TX and W_TX_PAYLOAD are sent with "R" instead of "W" because
	// they are on the highest byte-level in the nRF
	RF24ReadWrite(R, W_TX_PAYLOAD, WBuff, 5);
	
	_delay_ms(10);
	// CE high = transmit the data
	PORTB |= (1 << PORTB1);
	_delay_us(20);
	// CE low = stop transmitting
	PORTB &= ~(1 << PORTB1);
	_delay_ms(10);
}

/**
 * \brief Receive a data packet with the radio module
 * 
 * 
 * \return void
 */
void RF24ReceivePayload(void)
{
	// CE high = listens for data
	PORTB |= (1 << PORTB1);
	// listens for 1 sec
	_delay_ms(1000);
	// CE low = stop listening
	PORTB &= ~(1 << PORTB1);
}

/**
 * \brief Reset IRQ on the nRF24L01
 *
 * 
 * \return void
 */
void RF24ResetIRQ(void)
{
	_delay_us(10);
	// CSN low
	PORTB &= ~(1 << PORTB2);
	_delay_us(10);
	// Write to STATUS register
	SPIWriteByte(W_REGISTER + STATUS);
	_delay_us(10);
	// Reset all IRQ in STATUS register
	SPIWriteByte(0x70);
	_delay_us(10);
	// CSN high
	PORTB |= (1 << PORTB2);
}
