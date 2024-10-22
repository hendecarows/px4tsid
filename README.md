# px4tsid

BS,CSのトランスポンダに含まれるTSID一覧を取得するプログラムです。

BS帯域再編（トランスポンダ間の移動）に伴い、録画コマンドのチャンネル設定ファイルを修正する必要が出てきます。変更点を確認し手作業で修正する作業が意外と煩雑なことから可能な範囲で自動化することを目的としています。

動作確認環境は以下のとおりですが、px4_drvに対応したBS,CSの受信可能なチューナーであれば動作するはずです。

* Ubuntu 22.04
* px4_drvのfork ([https://github.com/tsukumijima/px4_drv][link_tsukumijima])
* DTV02-1T1S-U (Aなし初期型)

## インストール

### Linux

```console
git clone --recursive https://github.com/hendecarows/px4tsid.git
cd px4tsid
mkdir build
cd build
cmake ..
make -j
```

Linux環境のみが対象です。

## 使用方法

### TSID一覧の作成

BS,CSのトランスポンダに含まれるTSID一覧をJSON形式で保存します。

```console
px4tsid /dev/isdb2056video0 > tsids.json
```

移動前のTSID等不必要なTSIDが含まれている場合は、`--ignore`オプションで出力から除外して下さい。

```console
px4tsid --ignore 16529,18099,18130 /dev/isdb2056video0 > tsids.json
```

### チャンネル設定ファイルの作成

libdvbv5形式で出力します。

```console
px4tsid --format dvbv5 /dev/isdb2056video0 > dvbv5_channels_isdbs.conf
```

[BonDriver_DVB.conf][link_bdpl]形式で出力します。出力は`#ISDB_S`部分のみです。

```console
px4tsid --format bondvb /dev/isdb2056video0 > bondvb.txt
```

[BonDriver_LinuxPT.conf][link_bdpl]形式で出力します。出力は`#ISDB_S`部分のみです。

```console
px4tsid --format bonpt /dev/isdb2056video0 > bonpt.txt
```

[BonDriver_LinuxPTX.ini][link_bonptx]形式で出力します。

```console
px4tsid --format bonptx /dev/isdb2056video0 > bonptx.txt
```

[BonDriver_PX4-S.ChSet.txt][link_bonpx4]形式で出力します。

```console
px4tsid --format bonpx4 /dev/isdb2056video0 > bonpx4.txt
```

BonDriverのチャンネル名は、BS放送に対して`BSxx/TSx`形式とし、`BSxx`にはトランスポンダ番号(01-23)、
`TSx`にはTMCC信号内の相対TS番号(0-7)を設定します。

なお、Windowsで使用されている以下のツールとは`TSx`の考え方が異なるため一部のチャンネル名が一致しません。

* [PT1-PT2-Sample-200改][link_sample2]
* [PT3-Example-400改造版][link_sample3]
* [BSトラポンデータ作成][link_trapon]

[link_tsukumijima]: https://github.com/tsukumijima/px4_drv
[link_bdpl]: https://github.com/u-n-k-n-o-w-n/BonDriverProxy_Linux
[link_bonptx]: https://github.com/hendecarows/BonDriver_LinuxPTX
[link_bonpx4]: https://github.com/tsukumijima/px4_drv
[link_sample2]: https://www.axfc.net/u/3903918?key=PT2
[link_sample3]: https://www.axfc.net/u/3904457?key=PT3
[link_trapon]: https://www.axfc.net/u/4033018