Editor:
- Press CTRL + C to quit
- File provided via first argument
- Characters inserted after cursor and file getting updated live
- Cursor a thin line between two letters (An ASCII pipe).
- Backspace to delete the character before the cursor and file getting updated live
- Enter to insert a newline character
- Tab to insert a tab character
- Rendering done on file change
Version standard: vA.B.C, where A starts out as 0 for the non-functional stage, and increments as releases that break backwards compatibility are added (exception: When going from 0 to 1, it is influenced by whether the program is functional rather than a change breaking backwards compatibility). B starts out as 0 no matter what, and gets incremented as any feature that does not break backwards compatibility is added. C starts out as 0 no matter what, and gets incremented as any change in features is added (examples: bug fixes, other patches. Also, yes, improvement of existing features is counted as a B-class change, not C-class). Along with the version, a commit should also include what changed verbosely.
Code: Every line must have an empty line after it for readability. Comments, variable definition/modification blocks, brackets and blocks of lines starting with at least the same 3 characters are exempt. Brackets defining structures/blocks including but not limited to loops and functions must not have empty lines after an open bracket or an empty line before a closing bracket, but they must have empty lines before an open bracket and after a closing bracket. Variable definition/modification blocks must have a newline before and after them. The 3-character rule takes priority over brackets and brackets take priority over the variable definition/modification blocks.