find . -name "*.h" -o -name "*.cc" -o -name "*.S" -o -name "SConscript" -o -name "SConstruct" -o -name "*.py" > cscope.files
cscope -bkq -i cscope.files
ctags -R
