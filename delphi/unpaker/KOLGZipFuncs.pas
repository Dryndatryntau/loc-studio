unit KOLGZipFuncs;

interface

uses Kol, KOLZLib;

  function CompressStreamToStream(FSource: PStream; SSize: LongInt; var FDest: PStream): LongInt;
  function DecompressStreamToStream(FSource: PStream; Size, RealSize: LongInt; var FDest: PStream): LongInt;

const
  GZBufferSize=50000;
  crNone=0;
  crFastest=1;
  crDefault=2;
  crMax=3;

var
  GZBuffer: array[0..GZBufferSize-1]of byte;

implementation

function CompressStreamToStream(FSource: PStream; SSize: LongInt; var FDest: PStream): LongInt;
var
  CIn: PStream;
  O1,O2,i,TotalBlocks: LongInt;
begin
  FDest.Seek(0,spBegin);
  O1:=FDest.Position;
  CIn:=NewCompressionStream(clMax,FDest,nil);
  TotalBlocks:=SSize div GZBufferSize;
  if TotalBlocks=0 then
  begin
    FSource.Read(GZBuffer,SSize);
    CIn.Write(GZBuffer,SSize);
  end else
  begin
    for i:=1 to TotalBlocks do
    begin
      FSource.Read(GZBuffer,GZBufferSize);
      CIn.Write(GZBuffer,GZBufferSize);
    end;
    if SSize-TotalBlocks*GZBufferSize>0 then
    begin
      FSource.Read(GZBuffer,SSize-TotalBlocks*GZBufferSize);
      CIn.Write(GZBuffer,SSize-TotalBlocks*GZBufferSize);
    end;
  end;
  CIn.Free;
  O2:=FDest.Position;
  Result:=O2-O1;
end;

function DecompressStreamToStream(FSource: PStream; Size, RealSize: LongInt; var FDest: PStream): LongInt;
var
  COut: PStream;
  i,TotalBlocks: LongInt;
begin
  COut:=NewDecompressionStream(FSource,nil);
  TotalBlocks:=RealSize div GZBufferSize;
  if TotalBlocks=0 then
  begin
    COut.Read(GZBuffer,RealSize);
    FDest.Write(GZBuffer,RealSize);
  end else
  begin
    for i:=1 to TotalBlocks do
    begin
      COut.Read(GZBuffer,GZBufferSize);
      FDest.Write(GZBuffer,GZBufferSize);
    end;
    if RealSize-TotalBlocks*GZBufferSize>0 then
    begin
      COut.Read(GZBuffer,RealSize-TotalBlocks*GZBufferSize);
      FDest.Write(GZBuffer,RealSize-TotalBlocks*GZBufferSize);
    end;
  end;
  COut.Free;
end;

begin
end.
