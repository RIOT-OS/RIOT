RIOTBASE=$(git rev-parse --show-toplevel)
BOARD_FAMILIES_DOC=$(ls $RIOTBASE/boards/doc/*.md)

for FILE in $BOARD_FAMILIES_DOC
do
	CV=$(basename --suffix .md $FILE)
	HUMAN_NAME=$(head -n 1 $FILE)
	echo "@page board-family-$CV $HUMAN_NAME"
	cat $FILE
	#cat $FILE | awk "{if(NR==1){printf \"%s %s\n\", \$0, \"        {#board-family-$CV}\"}else{print}}"
	echo "Available boards"
	echo "================"
	sh $RIOTBASE/dist/tools/docgen/get-by-family.sh $CV | awk '$1="* @subpage board-"$1 {print $1}'
done
