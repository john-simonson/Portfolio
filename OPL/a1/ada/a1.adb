-- used http://rosettacode.org/wiki/Combinations#Ada for reference
-- equivalent to #include<stdio.h>
with Ada.Text_IO;                       use Ada.Text_IO;
with Ada.Integer_Text_IO;           use Ada.Integer_Text_IO;

procedure A1 is --main function
   generic -- functions similar to a class
      type Integers is range <>;
   package Combinations is --defines combination type
      type Combination is array (Positive range <>) of Integers;
      procedure First (X : in out Combination);
      procedure Next (X : in out Combination); 
      procedure Put (X : Combination);
   end Combinations;
 
   package body Combinations is --combination function
      procedure First (X : in out Combination) is
      begin
         X (1) := Integers'First;
         for I in 2..X'Last loop
            X (I) := X (I - 1) + 1;
         end loop;
      end First;
      procedure Next (X : in out Combination) is
      begin
         for I in reverse X'Range loop
            if X (I) < Integers'Val (Integers'Pos (Integers'Last) - X'Last + I) then
               X (I) := X (I) + 1;
               for J in I + 1..X'Last loop
                  X (J) := X (J - 1) + 1;
               end loop;
               return;
            end if;
         end loop;
         raise Constraint_Error; -- throm error
      end Next;
      procedure Put (X : Combination) is
      begin
         for I in X'Range loop
            Put (Integers'Image (X (I)));
         end loop;
      end Put;
   end Combinations; -- end combination function
 
   N : Integer; -- int n;
   K : Integer; -- int k;
begin
   Put("Enter an integer for N: "); -- print prompt
   Get(N); -- get user input for n
   Put("Enter an integer for K: "); -- print prompt
   Get(K); -- get user input for k
   declare
      subtype K_Range is Integer range 1 .. K;
      package Combination_K is new Combinations (K_Range);
      use Combination_K; -- use combination of length k
      X : Combination (1..N); -- use combination up to n
   begin -- print function
      First (X);
      loop
            Put (X); New_Line; --print and new line
            Next (X); -- back to loop
      end loop;
   end; -- end print function
exception -- for errors
   when Constraint_Error =>
      null;
end A1; -- end main
