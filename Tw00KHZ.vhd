-- *************************************************************
-- * FILENAME: slowclock.vhd                                   *
-- * PROVIDES:                                                 *
-- * - High frequency clocks prevent the use of LEDs to see    *
-- *   output signals changing because human eyes only resolve *
-- *   blinking light between 0 and approximately 30Hz.        *
-- * - LEDs are a good test method when sophisticated high     *
-- *   speed laboratory oscilloscopes and logic analyzers are  *
-- *   not available.                                          *
-- * - This file creates a slow 1 second clock by recognizing  *
-- *   that the 50MHz clock is pulsing 50E6 times per second.  *
-- *   A one second clock would pulse 1 time per second.       *
-- *   Thus, the one second pulse is high for 25E6 fast clocks *
-- *   and low for 25E6 fast clocks.                           *
-- *************************************************************
-- * TO USE:                                                   *
-- * - Add this component into the clock path of an FSM under  *
-- *   test. Connect the 50MHz clock to the input named CLK50  *
-- *   and connect output CLK1 to the machine clock input.     *
-- * - In Quartus, use structural VHDL to complete this port   *
-- *   mapping or use a schematic blueprint that drops both    *
-- *   this component and the machine component in as the top  *
-- *   level entity.                                           *
-- * - DO NOT ATTEMPT TO SIMULATE when this module is in place *
-- *   because it will take 50 million clock periods to get    *
-- *   one clock period on your machine. Your computer will    *
-- *   take forever to complete a simulation and consume much  *
-- *   hard drive space. Only simulate your machine before you *
-- *   add this component to the clock path.                   *
-- *************************************************************

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity SLOWCLK is
port( RST, CLK50MHZ: in std_logic;
      CLK1HZ:  inout std_logic);
end entity SLOWCLK;

architecture BEHAVIORAL of SLOWCLK is 

  signal COUNT: integer;
  -- 25E6 times high and 25E6 times low
  constant HALF: integer := 125;

begin
  
  UPDATE: process(RST,CLK50MHZ)
  begin
    if RST = '0' then COUNT <= 0; CLK1HZ <= '0';
    elsif rising_edge(CLK50MHZ) then COUNT <= COUNT + 1;
      if COUNT = HALF then CLK1HZ <= not CLK1HZ; COUNT <= 0;
      end if;
    end if;
  end process;
  
end architecture BEHAVIORAL;