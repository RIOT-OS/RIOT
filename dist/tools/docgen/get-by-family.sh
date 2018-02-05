RIOTBASE=$(git rev-parse --show-toplevel)

for DOCFILE in $(ls $RIOTBASE/boards/*/doc/board.md)
do
	FAMILY_NAME=$(head -n 1 $DOCFILE | sed -e "s/<!--//" -e "s/-->//")
	if [ "$FAMILY_NAME" = "$1" ]; then
		echo $DOCFILE | awk -F / '{ print $(NF-2) }'
	fi
done
