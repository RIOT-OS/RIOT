
1.概要

    ちび河童＆ちび馬は河童プロジェクトで作られたCUnit対応テストコード雛型
    生成ツールです．
    このちび河童＆ちび馬の吐き出すコードをEmbedded Unit用に書き換えました．

    オリジナルのちび河童＆ちび馬は以下のURLから入手可能です．

    河童プロジェクト
    http://sourceforge.jp/projects/cuppa/

    河童(CppUnit PreProcess Aid)
    http://www.unittest.org/


2.ファイル構成

    [tools]
    +- readme.txt   :このファイル
    +- makefile     :メイクファイル
    +- COPYING      :著作権表示
    +- [tbcuppa]    :ちび河童改ソースコード
    +- [tbuma]      :ちび馬改ソースコード


3.コンパイル

    toolsディレクトに移動して'make'コマンドを実行してください．
    toolsディレクトリに以下の４つのアプリケーションが作成されます．

    tcuppa  ：テストグループの雛形を生成します．
    bcuppa  ：tcuppaで生成されたテストグループを実行するmainを作成します．
    tuma    ：tcuppaで生成されたテストグループにテストを追加します．
    buma    ：bcuppaで生成されたmainにテスト実行コードを追加します．


4.使い方

4.1.tcuppa
    My.hに定義されている関数をテストするテストグループ MyTest を生成したい場合

    $ tcuppa My.h MyTest testXxx testYyy

    と入力する．そうするとMy.hをインクルードし空のテスト関数

        static void testXxx(void)
        static void testYyy(void)

    を実装したMyTest.c が生成される．ヘッダは省略可能,また複数指定可能.

4.2.bcuppa
    bcuppaはtcuppaによって生成されたテストグループを順次実行する
    メインルーチンを生成します．
    先ほど作成した MyTest を実行するコード AllTests を生成するには

    $ bcuppa AllTests MyTest

    と入力する．またカレントディレクトリに ATest.c BTest.cと在った場合

    $ bcuppa AllTests *Test.c

    のようなコマンドの指定が可能です．

4.3.tuma
    tcuppaで生成されたテストグループにテストを追加します．
    やはり先ほど作成した MyTest にテスト testZzz を追加したい場合、

    $ tuma MyTest testZzz

    と入力します．

4.4.buma
    bcuppaで生成された AllTests に実行コードを追加します．
    新しく YourTest と言うテストグループを生成します.

    $ tcuppa YourTest testXxx testYyy

    そして YourTest を AllTests に追加します．

    $ buma AllTests YourTest

------------------------------------------------------------------------------
$Id: readme.txt,v 1.1 2003/09/02 12:07:44 arms22 Exp $
