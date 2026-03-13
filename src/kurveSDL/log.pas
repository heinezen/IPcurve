unit log;

interface
uses constants,
     SysUtils;
procedure LogOut(s:string);

implementation

procedure LogOut(s:string);
var F:TextFile;
begin
 if ParamStr(1)='-log' then begin
  Assign(F,LogFile);
  if FileExists(LogFile) then Append(F)
                         else ReWrite(F);
  WriteLn(F,s);
  CloseFile(F);
 end;
end;

end.
 