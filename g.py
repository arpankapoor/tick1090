import csv,socket
from qpython import qconnection,qtype

SBS_HOST=('localhost',30003)
TP_HOST=('localhost',5010)
SBS_FIELDS=('mType','tType','sessionId','aircraftId','hexIdent','flightId','generatedDate','generatedTime','loggedDate','loggedTime','callSign','altitude','groundSpeed','track','lat','lon','verticalRate','squawk','alert','emergency','spi','isOnGround')
TBL_FIELDS=(
  ('hexIdent',qtype.QSYMBOL),
  ('mType',qtype.QSYMBOL),
  ('tType',qtype.QSHORT),
  ('callSign',qtype.QSYMBOL),
  ('altitude',qtype.QINT),
  ('groundSpeed',qtype.QINT),
  ('track',qtype.QINT),
  ('lat',qtype.QDOUBLE),
  ('lon',qtype.QDOUBLE),
  ('verticalRate',qtype.QINT),
  ('squawk',qtype.QSYMBOL),
  ('alert',qtype.QBOOL),
  ('emergency',qtype.QBOOL),
  ('spi',qtype.QBOOL),
  ('isOnGround',qtype.QBOOL)
)

def toQtype(val,qt):
 try:return qtype.PY_TYPE[qt](val)
 except:return qtype.qnull(qt)

def toTblRow(sbsRow):
 return [toQtype(sbsRow[fd[0]],fd[1]) for fd in TBL_FIELDS]

def main():
 with socket.create_connection(SBS_HOST) as sc,qconnection.QConnection(TP_HOST[0],TP_HOST[1]) as tp:
  reader=csv.DictReader(sc.makefile(),fieldnames=SBS_FIELDS)
  for row in reader:tp.sync('.u.upd',toQtype('fd',qtype.QSYMBOL),toTblRow(row))

if __name__=="__main__":main()
