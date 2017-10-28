library ieee;
use ieee.std_logic_1164.all;

entity tstreg is 
	port( A : in std_logic_vector(7 downto 0);
			F : out std_logic);
end entity tstreg;

architecture behavioral of tstreg is 

begin 
	with A select F <= 
		'1' when b"11111111",
		'0' when others;
end architecture behavioral;