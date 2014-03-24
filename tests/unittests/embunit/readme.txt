

                        == Embedded Unit ==
               https://sourceforge.jp/projects/embunit/

------------------------------------------------------------------------------

Embedded UnitはC言語を使った組み込み系開発向けのテストユニットフレームワークで
す。C標準ライブラリを使わないので実行資源の少ないターゲット環境で動作可能です。
また、malloc関数も使用していません。すべてのオブジェクトはROM領域に確保されま
す。

------------------------------------------------------------------------------

1.ディレクトリ・ファイル構成
    [embUnit]
     +- COPYING         : ライセンス
     +- makefile        : メイクファイル
     +- readme.txt      : このファイル
     +- [embUnit]       : Embedded Unit ライブラリソース
     +- [msvc]          : MSVC++6.0 Project
     +- [samples]       : Embedded Unit サンプルソース
     +- [tests]         : Embedded Unit テストソース
     +- [tools]         : テストコードの雛形生成ツール


2.ターゲットシステムと開発環境

    Embedded Unitの実行環境として次のものを想定している

    ・2KB以上のROM領域、128b以上のスタック領域を使用可能な環境
    ・標準Cライブラリが使用できない、したくない環境

    また、開発環境として次のものを想定している

    ・Cコンパイラが使える環境

    補足

    Embedded Unitの開発は次の環境で行われている

    ・Microsoft Windows XP Professional
    ・VC++.NET or cygwin 1.3.22 + gcc 3.2
	・Microsoft Windows 98
	・VC++6.0
    ・Apple Computer MacOS X 10.1.5
    ・Project Builder 1.1.1 (gcc 2.95.2)


3.コンパイル
    3.1.UNIX系(cygwin)
        - 必要に応じてembUnit/config.hを編集,以下のコマンドを実行.

            $ make

        - テストフレーム自身のテストを実行.

            $ make test

        - サンプルを実行.

            $ make samples

    3.2.MSVC++6.0
        - msvc/embUnit.dsw を開いて各プロジェクトをビルドしてください.

    3.3.MacOSX
        - 必要に応じてembUnit/config.hを編集,CC変数をccに置き換えコマンド実行.

            $ make CC=cc


4.サポートする機能の概要

    ・検証マクロ
      TEST_ASSERT_EQUAL_STRING･･･文字列の検証
      TEST_ASSERT_EQUAL_INT･･･int型変数の検証
      TEST_ASSERT_NULL･･･NULLポインタの検証
      TEST_ASSERT_NOT_NULL･･･非NULLポインタの検証
      TEST_ASSERT_MESSAGE･･･検証とメッセージ
      TEST_ASSERT･･･検証
      TEST_FAIL･･･失敗

    ・ヘルパーマクロ
      テストを記述するのに必要な作業をヘルパーマクロで簡略化します．


5.将来サポートする機能の概要

    ・CUITestRunner
      対話型のTestRunnerの開発を予定しています．
      ※標準Cライブラリが使える環境を想定して開発を行います．
        可能なら標準Cライブラリを使わないバージョンも開発します．

    ・GUITestRunner
      GUIを利用したTestRunnerです．

    ・textui
      Text形式、XML形式、Compiler形式のテスト結果を出力するTestRunnerを開発し
      ます．


6.制限事項

    Embedded Unitはテスト結果の出力に標準ライブラリのprintf関数を使用していま
    す．もし標準ライブラリのprintf関数を使用したくない場合は、

        void stdimpl_print(const char* string)
        ※stdimpl_print関数は文字列の終わりに改行を出力しない関数

    という関数を実装し、コンパイルオプションに"-DNO_STDIO_PRINTF"を追加してく
    ださい．もしくはconfigヘッダの以下のコメントはずしてください．

        /*#define NO_STDIO_PRINTF*/


7.ライセンス

    7.1.ライセンス
        Embedded Unitは
            MIT/X Consortium License
        に従うものとします．

    7.2.日本語訳(参考)
        http://xjman.dsl.gr.jp/xf86_3/CPYRIGHT-2.html

    7.3.さまざまなライセンスとそれらについての解説
        http://www.gnu.org/licenses/license-list.ja.html


------------------------------------------------------------------------------
$Id: readme.txt,v 1.10 2003/09/16 11:41:48 arms22 Exp $
