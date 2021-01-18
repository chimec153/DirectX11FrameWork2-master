cd..
cd..

xcopy	.\GameEditor\Include\Client\*.* .\Engine\Include\Client\	/s /y /d
xcopy	.\GameEditor\Include\*.h .\Engine\Include\	/s /y /d
xcopy	.\GameEditor\Include\*.cpp .\Engine\Include\	/s /y /d

xcopy	.\GameEditor\Bin\Resource\*.*	.\Engine\Bin\Resource\	/s /y /d
xcopy	.\GameEditor\Bin\Resource\*.*	.\GameClient\Bin\Resource\	/s /y /d
xcopy	.\GameEditor\Bin\Resource\*.*	.\Engine\Bin\Sound\	/s /y /d
xcopy	.\GameEditor\Bin\Sound\*.*	.\GameClient\Bin\Sound\	/s /y /d
xcopy	.\GameEditor\Bin\Resource\*.*	.\Engine\Bin\Data\	/s /y /d
xcopy	.\GameEditor\Bin\Data\*.*	.\GameClient\Bin\Data\	/s /y /d