RIOTBASE=$(git rev-parse --show-toplevel)
DOC_FILES=$(ls -a $RIOTBASE/boards/doc/*.md)

echo "Supported Boards                                                  {#boards-wiki}"
echo "================"
echo ""

for FILE in $DOC_FILES
do
	CV=$(basename --suffix .md $FILE)
	echo "* Family: @subpage board-family-$CV"
	sh $RIOTBASE/dist/tools/docgen/get-by-family.sh $CV | awk '$1="    * @ref board-"$1 {print $1}'
done
