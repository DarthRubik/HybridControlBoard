library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;
use ieee.numeric_std.all;

entity servo is 
port(eightBitBus : in std_logic_vector(7 downto 0);
		clk, rst   : in std_logic;
	 outputSignal : out std_logic);
end entity servo;

architecture behavioral of servo is 

signal count : integer range 0 to 3999;
signal endOfPulse : integer range 0 to 499;

begin

	memory: process(clk, rst, count, endOfPulse)
	begin
	if rst = '0' then count <= 0;
	elsif rising_edge(clk) then 
		if count < 3999 then 
			count <= count + 1;
		else 
			count <= 0;
		end if;
	end if;
	end process;
	
	
	pulsing: process(eightBitBus,rst,clk,count,endOfPulse)
	begin
	if rst = '0' then outputSignal <= '0';
	elsif rising_edge(clk) then 
	if eightBitBus < 200 then 
		endOfPulse <= 200 + conv_integer(eightBitBus);
		if count < endOfPulse then 
			outputSignal <= '1';
		else 
			outputSignal <= '0';
		end if;
	else 
		outputSignal <= '0';
	end if;
	end if;
	end process;
		

end architecture behavioral;