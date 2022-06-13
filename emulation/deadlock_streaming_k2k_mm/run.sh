find ./.run/*/ -name simulate.log -exec grep 'Inter Kernel deadlock has been detected' {} \; > output.txt

File="./output.txt"
string="Inter Kernel deadlock has been detected"

echo "Waiting for deadlock detection message ..."

if grep -q "$string" $File; then
    echo "FOUND: "$string""
else
    echo "ERROR: Deadlock not detected"
fi
