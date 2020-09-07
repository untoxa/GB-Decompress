{$apptype console}

uses windows, classes, sysutils;

function stripname(const aname: ansistring): ansistring;
var p : integer;
begin
  p:= pos('.', aname);
  if p > 0 then result:= copy(aname, 1, p - 1) else result:= aname;
end;

procedure outputstring(aos: tStream; const astr: ansistring);
const crlf : array[0..1] of ansichar = #$0d#$0a;
begin
  if assigned(aos) and (length(astr) > 0) then begin
    aos.write(astr[1], length(astr));
    aos.write(crlf, sizeof(crlf));
  end;
end;

var so  : tMemoryStream;
    i   : longint;
    tmp : ansistring;
begin
  if (paramcount = 2) then begin
    so:= tMemoryStream.create;
    try
      with tMemoryStream.Create do try
        LoadFromFile(paramstr(1));
        outputstring(so, format('const unsigned char %s[] = {', [stripname(extractfilename(paramstr(1)))]));
        setlength(tmp, 0);
        for i:= 0 to size - 1 do begin
           if (i mod 16 = 0) then begin
             outputstring(so, tmp);
             setlength(tmp, 0);
           end;
           tmp:= tmp + '0x' + IntToHex(byte(pAnsiChar(memory)[i]), 2) + ',';
        end;
        outputstring(so, tmp);
        outputstring(so, '};');
        so.SaveToFile(paramstr(2));
      finally free; end;
    finally freeandnil(so); end;
  end else begin
    writeln('Usage: bin2c <input_file> <output_file>');
    Halt(1);
  end;
end.