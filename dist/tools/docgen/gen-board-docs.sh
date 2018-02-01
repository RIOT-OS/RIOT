RIOTBASE=$(git rev-parse --show-toplevel)

for DOCFILE in $(ls $RIOTBASE/boards/*/doc/board.md)
do
	HUMAN_NAME=$(sed -n "2 p" $DOCFILE)
	CV=$(echo $DOCFILE | awk -F / '{ print $(NF-2) }')
	echo "@page board-$CV $HUMAN_NAME"
	cat $DOCFILE
done
