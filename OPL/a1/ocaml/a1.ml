(*Used http://rosettacode.org/wiki/Combinations#OCaml for reference*)
let combinations m n = (*function to find the combinations*)
  let rec c = function
    | (0,_) -> [[]] (*adds the brackets*)
    | (_,0) -> []
    | (p,q) -> List.append (*gets the entries and appends them to a list*)
               (List.map (List.cons (n-(q-1))) (c (p-1, q-1)))
               (c (p , q-1))
  in c (m , n) (*calls c for given vals*)

let print i j = (*print function*)
  let rec print_list = function
    | [] -> print_newline () (*print new line*)
    | hd :: tl -> print_int hd ; print_string " "; print_list tl
  in List.iter print_list (combinations i j) (*print combination list*)

let () =
	print_string "Enter k and n: \n"; flush stdout;; (*print prompt and flush stdout*)
	let i = read_int () in (*get i (aka k)*)
	let j = read_int () in (*get j (aka n)*)
	print i j (*call print with user input*)