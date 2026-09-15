#ifdef FUELLSTANDSMESSER3_UNITY_BUILD

#include "WebServerModule.h"

static void sendLocalizedHtml(int statusCode, String html) {
  localizeWebHtml(html);
  server->send(statusCode, "text/html; charset=utf-8", html);
}

// ============================================================================
// WEB COMMON
// ============================================================================

String pageHeader(
  const String &title
) {

  String html;

  html.reserve(3000);

  html += "<!DOCTYPE html>";
  html += "<html lang='";
  html += languageCode();
  html += "'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' ";
  html += "content='width=device-width,initial-scale=1'>";
  html += "<title>";
  html += htmlEscape(title);
  html += "</title>";
  // Eingebettetes Tank-Favicon; keine zusätzliche SPIFFS-Datei erforderlich.
  html += "<link rel='icon' type='image/svg+xml' href='data:image/svg+xml,";
  html += "%3Csvg xmlns=%22http://www.w3.org/2000/svg%22 viewBox=%220 0 64 64%22%3E";
  html += "%3Crect width=%2264%22 height=%2264%22 rx=%2214%22 fill=%22%230f1b2b%22/%3E";
  html += "%3Crect x=%2215%22 y=%2214%22 width=%2234%22 height=%2240%22 rx=%226%22 fill=%22none%22 stroke=%22%23fff%22 stroke-width=%224%22/%3E";
  html += "%3Cpath d=%22M18 36 Q25 31 32 36 T46 36 V50 H18Z%22 fill=%22%234da6ff%22/%3E";
  html += "%3Cpath d=%22M25 10 H39 V15 H25Z%22 fill=%22%23fff%22/%3E";
  html += "%3C/svg%3E'>";

  html += "<style>.pctValue{display:flex;align-items:center;justify-content:center;font-size:clamp(26px,5vw,42px);font-weight:800;line-height:1.1;min-height:52px;white-space:nowrap;text-shadow:0 1px 2px rgba(0,0,0,.65)}"
  ".pctLabel{font-size:14px;font-weight:700;opacity:.9;margin-left:4px}";

  html +=
    "body{font-family:Arial,sans-serif;"
    "background:#111;color:#eee;margin:0;padding:20px;}";

  html +=
    ".wrap{max-width:1000px;margin:auto;}";

  html +=
    ".card{background:#1d1d1d;padding:20px;"
    "margin-bottom:20px;border-radius:12px;"
    "box-shadow:0 2px 8px #000;}";

  html +=
    "h1,h2{color:#4dd0e1;}";

  html +=
    "input,select{width:100%;box-sizing:border-box;"
    "padding:10px;margin:5px 0 14px;"
    "background:#2a2a2a;color:#fff;"
    "border:1px solid #555;border-radius:6px;}";

  html +=
    "button,.btn{display:inline-block;"
    "padding:11px 16px;background:#008c9e;"
    "color:white;border:0;border-radius:6px;"
    "text-decoration:none;cursor:pointer;margin:4px;}";

  html +=
    ".danger{background:#b3261e;}";

  html +=
    "table{width:100%;border-collapse:collapse;}";

  html +=
    "td,th{padding:8px;border-bottom:1px solid #444;"
    "text-align:left;}";

  html +=
    ".ok{color:#65e572;}"
    ".bad{color:#ff6565;}"
    ".grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:12px;}"
    ".metric-card{background:#292929;padding:14px;border-radius:10px;}"
    ".metric-card h3{margin:0 0 8px;font-size:1rem;color:#4dd0e1;}"
    ".metric-card div{font-size:1.2rem;font-weight:700;}"
    ".nav{display:grid;grid-template-columns:repeat(4,minmax(0,1fr));gap:8px;margin-bottom:16px;}"
    ".nav a{display:block;text-align:center;padding:11px 8px;background:#292929;border:1px solid #444;border-radius:8px;color:#eee;text-decoration:none;font-weight:700;margin:0;}"
    ".nav a.active{background:#008c9e;border-color:#008c9e;}"
    ".footer-nav{display:flex;flex-wrap:wrap;gap:8px;}"
    ".topbar{display:flex;justify-content:space-between;align-items:center;margin-bottom:14px;gap:10px;}"
    ".topbar h1,.topbar h2{margin:0;}"
    ".links{display:flex;flex-wrap:wrap;gap:8px;}"
    "@media(max-width:760px){.grid{grid-template-columns:1fr;}.topbar{flex-direction:column;align-items:flex-start;}.nav{grid-template-columns:repeat(2,1fr);}}";

  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<div class='wrap'>";

  return html;
}

String pageFooter() {

  String html;

  html += "<div style='color:#777;text-align:center'>";
  html += DEVICE_NAME;
  html += " ";
  html += FW_VERSION;
  html += "</div>";

  html += "</div>";
  html += "</body>";
  html += "</html>";

  return html;
}

// ============================================================================
// WEB STATUS
// ============================================================================

void handleRoot() {
  String html = pageHeader("Fuellstandsmesser3 V7.3.8");
  html.reserve(18000);
  html += R"HTML(
<style>
.dash{display:grid;grid-template-columns:repeat(12,1fr);gap:12px}.tile{background:#1d1d1d;border-radius:14px;padding:15px;box-shadow:0 2px 8px #000}.hero{grid-column:span 6;display:flex;gap:22px;align-items:center}.side{grid-column:span 3}.wide{grid-column:span 12}.statusSide{grid-column:span 3}.infoSide{grid-column:span 12}.statusSide .statusGrid{grid-template-columns:1fr}.statusSide .status{text-align:left}.statusSide .status small{display:inline;margin-left:4px}.infoGrid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:12px}.infoGrid .tile{box-shadow:none}.label{font-size:.78rem;color:#aaa;text-transform:uppercase;letter-spacing:.08em}.big{font-size:3.2rem;font-weight:800;line-height:1.05;margin:7px 0}.unit{font-size:1.05rem;color:#aaa}.muted{color:#999}.metrics{display:grid;grid-template-columns:repeat(2,1fr);gap:9px;margin-top:10px}.metric{background:#292929;border-radius:10px;padding:10px}.metric b{display:block;font-size:1.12rem;margin-top:3px}.bar{height:16px;background:#333;border-radius:9px;overflow:hidden;margin-top:9px}.barFill{height:100%;width:0;transition:width .5s}.tank{width:145px;height:205px;border:4px solid #777;border-radius:14px;position:relative;overflow:hidden;background:#111;flex:none}.tankFill{position:absolute;left:0;right:0;bottom:0;height:0;transition:height .5s}.tankMark{position:absolute;left:8px;right:8px;top:50%;height:1px;background:#777;opacity:.5}.tankScale{position:absolute;z-index:2;left:15%;right:15%;top:40%;height:20%;display:flex;align-items:center;justify-content:center;font-size:1.45rem;font-weight:800;background:rgba(0,0,0,.6);border-radius:9px;text-shadow:0 2px 4px #000}.statusGrid{display:grid;grid-template-columns:repeat(7,1fr);gap:8px}.status{background:#292929;border-radius:10px;padding:10px;text-align:center}.dot{display:inline-block;width:11px;height:11px;border-radius:50%;margin-right:5px;vertical-align:middle}.green{background:#42d65b}.red{background:#ff4d4d}.orange{background:#ffad33}.blue{background:#4da6ff}.gray{background:#888}.status small{display:block;color:#aaa;margin-top:5px}.chartHead{display:flex;justify-content:space-between;align-items:center;gap:10px;flex-wrap:wrap}.periods{display:flex;gap:6px;flex-wrap:wrap}.periodBtn{background:#292929;border:1px solid #4b4b4b;border-radius:999px;padding:6px 11px;color:#eee;cursor:pointer}.periodBtn.active{background:#1769aa;border-color:#1769aa}.chartWrap{position:relative;width:100%;height:280px}.chart{width:100%;height:100%;display:block}.tooltip{position:absolute;display:none;pointer-events:none;min-width:170px;max-width:230px;background:#101418;border:1px solid #4d5965;border-radius:9px;padding:9px;box-shadow:0 4px 14px #000;font-size:12px;z-index:5}.tooltip b{color:#fff}.legend{display:flex;gap:14px;flex-wrap:wrap;color:#aaa;font-size:.8rem;margin-top:7px}.legend i{display:inline-block;width:10px;height:10px;border-radius:50%;margin-right:4px}.linkrow{display:flex;gap:7px;flex-wrap:wrap}.footerNote{text-align:center;color:#777;padding:8px 0}
@media(max-width:800px){.hero,.side,.wide,.statusSide,.infoSide{grid-column:span 12}.hero{flex-direction:column;text-align:center}.tank{width:120px;height:165px}.big{font-size:2.7rem}.statusSide .statusGrid{grid-template-columns:repeat(4,1fr)}.statusSide .status{text-align:center}.statusSide .status small{display:block;margin-left:0}.infoGrid{grid-template-columns:1fr}}
@media(max-width:520px){.metrics{grid-template-columns:1fr}.statusGrid{grid-template-columns:repeat(2,1fr)}.chartWrap{height:240px}}
</style>
<div class='nav'><a class='active' href='/'>Dashboard</a><a href='/history'>Historie</a><a href='/settings'>Einstellungen</a><a href='/systemstatus'>System</a></div>
<div class='dash'>
<div class='tile hero'><div class='tank'><div id='tankFill' class='tankFill'></div><div class='tankMark'></div><div id='tankScale' class='tankScale'>-- %</div></div><div style='flex:1;width:100%'><div class='label'>Füllstand aktuell</div><div><span id='liters' class='big'>--</span> <span class='unit'>Liter</span></div><div class='bar'><div id='levelBar' class='barFill'></div></div><div class='metrics'><div class='metric'><span class='muted'>Füllhöhe</span><b id='height'>-- mm</b></div><div class='metric'><span class='muted'>Sensorabstand</span><b id='distance'>-- mm</b></div><div class='metric'><span class='muted'>Tankfaktor</span><b id='litersPerMm'>-- L/mm</b></div></div><div class='muted' id='updated'>Warte auf Messdaten …</div></div></div>
<div class='tile side'><h2>Verbrauch</h2><div class='metric'><span class='muted'>Heute</span><b id='consToday'>-- L</b></div><div class='metric'><span class='muted'>7 Tage</span><b id='consWeek'>-- L</b></div><div class='metric'><span class='muted'>30 Tage</span><b id='consMonth'>-- L</b></div><div class='metric'><span class='muted'>365 Tage</span><b id='cons365'>-- L</b></div><div class='metric'><span class='muted'>Ø 30 Tage</span><b id='consAvg'>-- L/Tag</b></div></div>
<div class='tile statusSide'><h2>Status</h2><div class='statusGrid'><div class='status'><span id='dotWifi' class='dot gray'></span>WLAN<small id='txtWifi'>--</small></div><div class='status'><span id='dotMqtt' class='dot gray'></span>MQTT<small id='txtMqtt'>--</small></div><div class='status'><span id='dotTof' class='dot gray'></span>ToF<small id='txtTof'>--</small></div><div class='status'><span id='dotBme' class='dot gray'></span>BME280<small id='txtBme'>--</small></div><div class='status'><span id='dotSpiffs' class='dot gray'></span>SPIFFS<small id='txtSpiffs'>--</small></div><div class='status'><span id='dotHist' class='dot gray'></span>Historie<small id='txtHist'>--</small></div><div class='status'><span id='dotOta' class='dot blue'></span>OTA<small id='txtOta'>Bereit</small></div></div></div>
<div class='tile wide'><div class='chartHead'><h2 style='margin:0'>Füllstand & Verbrauch</h2><div class='periods'><button class='periodBtn' data-period='183'>½ Jahr</button><button class='periodBtn active' data-period='365'>1 Jahr</button><button class='periodBtn' data-period='1825'>5 Jahre</button><button class='periodBtn' data-period='3650'>10 Jahre</button></div></div><div class='chartWrap'><canvas id='chart' class='chart'></canvas><div id='tip' class='tooltip'></div></div><div class='legend'><span><i style='background:#4da6ff'></i>Füllstand</span><span><i style='background:#ffb52e'></i>Verbrauch</span><span><i style='background:#42d65b'></i>Nachfüllung</span></div></div>
<div class='tile infoSide'><h2>Umgebung</h2><div class='infoGrid'><div class='metric'><span class='muted'>Temperatur</span><b id='temp'>-- °C</b></div><div class='metric'><span class='muted'>Feuchte</span><b id='hum'>-- %</b></div><div class='metric'><span class='muted'>Druck</span><b id='press'>-- hPa</b></div><div class='metric'><span class='muted'>Taupunkt</span><b id='dew'>-- °C</b></div></div></div><div class='tile infoSide'><h2>System</h2><div class='infoGrid'><div class='metric'><span class='muted'>Datum / Uhrzeit</span><b id='dt'>--</b></div><div class='metric'><span class='muted'>RSSI</span><b id='rssi'>-- dBm</b></div><div class='metric'><span class='muted'>Firmware</span><b>V7.3.8</b></div></div></div>
</div><div class='footerNote'>Fuellstandsmesser3 V7.3.8 · Dashboard</div>
<script>
(function(){const $=id=>document.getElementById(id),fmt=(v,d=1)=>Number.isFinite(Number(v))?Number(v).toFixed(d):'--';let period=365,items=[],geom=null;
function dot(id,state){const e=$(id);if(e)e.className='dot '+(state?'green':'red')}
function status(d){if(!d)return;const p=Math.max(0,Math.min(100,Number(d.level_percent)||0));$('tankScale').textContent=fmt(p)+' %';$('liters').textContent=fmt(d.level_liters);$('height').textContent=fmt(d.level_height_mm)+' mm';$('distance').textContent=fmt(d.filtered_distance_mm)+' mm';$('litersPerMm').textContent=fmt(d.liters_per_mm,2)+' L/mm';$('consToday').textContent=fmt(d.consumption_today_l)+' L';$('consWeek').textContent=fmt(d.consumption_7d_l)+' L';$('consMonth').textContent=fmt(d.consumption_30d_l)+' L';$('cons365').textContent=fmt(d.consumption_365d_l)+' L';$('consAvg').textContent=fmt(d.consumption_avg_30d_l_day)+' L/Tag';$('temp').textContent=fmt(d.temperature_c)+' °C';$('hum').textContent=fmt(d.humidity_percent)+' %';$('press').textContent=fmt(d.pressure_hpa)+' hPa';$('dew').textContent=fmt(d.dew_point_c)+' °C';$('dt').textContent=(d.date||'--')+' '+(d.time||'');$('rssi').textContent=fmt(d.rssi,0)+' dBm';dot('dotWifi',!!d.wifi_connected);dot('dotMqtt',!!d.mqtt_connected);dot('dotTof',!!d.vl_ok);dot('dotBme',!!d.bme280_ok);dot('dotSpiffs',!!d.spiffs_ready);dot('dotHist',!!d.history_ready);$('txtWifi').textContent=d.wifi_connected?'OK':'Fehler';$('txtMqtt').textContent=d.mqtt_connected?'OK':'Fehler';$('txtTof').textContent=d.vl_ok?'OK':'Fehler';$('txtBme').textContent=d.bme280_ok?'OK':'Fehler';$('txtSpiffs').textContent=d.spiffs_ready?'OK':'Fehler';$('txtHist').textContent=d.history_ready?'OK':'Fehler';const fill=$('tankFill'),bar=$('levelBar');fill.style.height=p+'%';bar.style.width=p+'%';const col=p<20?'#ff4d4d':(p<40?'#ffad33':'#42d65b');fill.style.background=col;bar.style.background=col;$('updated').textContent='Letzte Aktualisierung: '+(d.time||new Date().toLocaleTimeString('de-DE'))}
function setPeriod(n){period=Number(n);document.querySelectorAll('.periodBtn').forEach(b=>b.classList.toggle('active',Number(b.dataset.period)===period));loadHistory()}
document.querySelectorAll('.periodBtn').forEach(b=>b.onclick=()=>setPeriod(b.dataset.period));
function draw(){const c=$('chart'),tip=$('tip');if(!c)return;const r=c.getBoundingClientRect(),dpr=Math.max(1,devicePixelRatio||1),w=Math.max(320,Math.floor(r.width)),h=Math.floor(r.height);c.width=Math.round(w*dpr);c.height=Math.round(h*dpr);const x=c.getContext('2d');x.setTransform(dpr,0,0,dpr,0,0);x.clearRect(0,0,w,h);const pl=42,pr=12,pt=18,pb=28,iw=w-pl-pr,ih=h-pt-pb;if(!items.length){x.fillStyle='#777';x.fillText('Keine Verlaufsdaten vorhanden',pl,pt+20);return}const ts=items.map(a=>a.time),t0=Math.min.apply(null,ts);let t1=Math.max.apply(null,ts);if(t1<=t0)t1=t0+86400000;const px=t=>pl+((t-t0)/(t1-t0))*iw,py=p=>pt+ih-(Math.max(0,Math.min(100,p))/100)*ih;x.strokeStyle='#333';[0,25,50,75,100].forEach(v=>{const y=py(v);x.beginPath();x.moveTo(pl,y);x.lineTo(w-pr,y);x.stroke();x.fillStyle='#777';x.font='10px Arial';x.fillText(v+'%',5,y+3)});const sampleDays=period>365?Math.max(1,Math.ceil(period/400)):1,maxLineGap=Math.max(129600000,sampleDays*86400000*1.75);x.beginPath();items.forEach((a,i)=>{const q=px(a.time),y=py(a.percent),gap=i?(a.time-items[i-1].time):0;(i&&gap<=maxLineGap)?x.lineTo(q,y):x.moveTo(q,y)});x.strokeStyle='#4da6ff';x.lineWidth=2;x.stroke();const maxC=Math.max.apply(null,[1].concat(items.map(a=>Number(a.consumedLiters)||0)));items.forEach(a=>{const q=px(a.time),y=py(a.percent),bh=((Number(a.consumedLiters)||0)/maxC)*(ih*.35);x.fillStyle='#ffb52e';x.fillRect(q-1,pt+ih-bh,2,bh);const src=Number(a.source)||0;x.fillStyle=src===1?'#ffd166':(src===2?'#ff6b6b':'#4da6ff');x.beginPath();if(src===1){x.arc(q,y,4,0,Math.PI*2);x.strokeStyle=x.fillStyle;x.lineWidth=2;x.stroke()}else if(src===2){x.moveTo(q,y-4);x.lineTo(q+4,y+4);x.lineTo(q-4,y+4);x.closePath();x.fill()}else{x.arc(q,y,3,0,Math.PI*2);x.fill()}if(Number(a.refillLiters)>0){x.fillStyle='#42d65b';x.beginPath();x.arc(q,pt+ih-6,5,0,Math.PI*2);x.fill()}});x.fillStyle='#777';x.font='10px Arial';[0,.5,1].forEach(f=>{const d=new Date(t0+f*(t1-t0));x.fillText(d.toLocaleDateString('de-DE',{month:'2-digit',year:'2-digit'}),Math.max(pl,Math.min(w-45,px(t0+f*(t1-t0))-18)),h-6)});geom={t0,t1,px}}
const c=$('chart'),tip=$('tip');
if(c&&tip){
 c.addEventListener('mousemove',e=>{if(!geom||!items.length)return;const rect=c.getBoundingClientRect(),mx=e.clientX-rect.left;let best=null,bd=9999;items.forEach(a=>{const dx=Math.abs(geom.px(a.time)-mx);if(dx<bd){bd=dx;best=a}});if(!best||bd>24){tip.style.display='none';return}const d=new Date(best.time);const srcName=Number(best.source)===1?'Import':(Number(best.source)===2?'Test':'Gemessen');tip.innerHTML='<b>'+d.toLocaleDateString('de-DE')+'</b><br>Füllstand: '+fmt(best.percent)+' %<br>Menge: '+fmt(best.liters)+' L<br>Verbrauch: '+fmt(best.consumedLiters)+' L<br>Quelle: '+srcName+(Number(best.refillLiters)>0?'<br><span style="color:#42d65b">Nachfüllung: +'+fmt(best.refillLiters)+' L</span>':'');tip.style.display='block';tip.style.left=Math.min(rect.width-180,Math.max(6,mx+12))+'px';tip.style.top='10px'});
 c.addEventListener('mouseleave',()=>tip.style.display='none');
}
async function loadHistory(){try{const r=await fetch('/api/history?days='+period+'&ts='+Date.now(),{cache:'no-store'});if(r.ok){const j=await r.json();items=j.items||[];draw()}}catch(e){}}
async function refresh(){try{const r=await fetch('/api/status?ts='+Date.now(),{cache:'no-store',headers:{'Accept':'application/json'}});const raw=await r.text();if(!r.ok)throw new Error('HTTP '+r.status);let d;try{d=JSON.parse(raw)}catch(e){throw new Error('Ungültiges JSON: '+raw.substring(0,120))}status(d)}catch(e){$('updated').textContent='API-Fehler: '+e.message;$('updated').style.color='#ff4d4d'}}
draw();refresh();loadHistory();setInterval(refresh,10000);setInterval(loadHistory,30000);addEventListener('resize',draw);setTimeout(draw,100)})();
</script>
)HTML";
  sendLocalizedHtml(200, html);
}

// ============================================================================
// WEB CONFIG
// ============================================================================

String checked(
  bool value
) {
  return value ? " checked" : "";
}

String selected(
  bool value
) {
  return value ? " selected" : "";
}

String formatI2CAddress(uint8_t address) {
  char buf[8];
  snprintf(buf, sizeof(buf), "0x%02X", address);
  return String(buf);
}

bool parseI2CAddress(const String &input, uint8_t &address) {
  String value = input;
  value.trim();
  if (value.length() == 0) {
    return false;
  }

  char *endPtr = nullptr;
  unsigned long parsed = strtoul(value.c_str(), &endPtr, 0);
  if (endPtr == value.c_str() || *endPtr != '\0' || parsed > 0x7F) {
    return false;
  }

  address = (uint8_t)parsed;
  return true;
}

void handleSettings() {

  String html = pageHeader("Einstellungen");
  html.reserve(26000);

  html += R"HTML(
<style>
.settingsNav{display:grid;grid-template-columns:repeat(4,minmax(0,1fr));gap:8px;margin-bottom:16px}
.settingsNav a{display:block;text-align:center;padding:11px 8px;background:#292929;border:1px solid #444;border-radius:8px;color:#eee;text-decoration:none;font-weight:700}
.settingsNav a.active{background:#008c9e;border-color:#008c9e}
.settingsGrid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:14px;align-items:start}
.settingsBlock{background:#1d1d1d;border-radius:12px;padding:16px;box-shadow:0 2px 8px #000}
.settingsBlock h2{margin:0 0 5px;color:#4dd0e1;font-size:1.15rem}
.settingsBlock p{margin:0 0 12px;color:#999;font-size:.86rem;line-height:1.4}
.settingsBlock label{display:block;color:#ddd;font-size:.9rem;margin-top:9px}
.settingsBlock input,.settingsBlock select{width:100%;box-sizing:border-box;padding:9px 10px;margin:4px 0 5px;background:#2a2a2a;color:#fff;border:1px solid #555;border-radius:6px}
.settingsBlock input[type=checkbox]{width:auto;margin-right:7px;transform:scale(1.1)}
.settingsHelp{display:block;color:#888;font-size:.76rem;margin-bottom:5px}
.settingsWide{grid-column:1/-1}
.settingsActions{position:sticky;bottom:0;z-index:20;background:rgba(17,17,17,.96);padding:10px;border-top:1px solid #333;margin-top:14px;text-align:center}
.saveBtn{background:#008c9e!important;font-size:1rem;font-weight:700;padding:12px 24px!important}
.warnBox{background:#3a2b13;border:1px solid #8b641e;border-radius:8px;padding:10px;color:#f0c36a;font-size:.82rem;margin-top:10px}
@media(max-width:760px){.settingsGrid{grid-template-columns:1fr}.settingsWide{grid-column:auto}.settingsNav{grid-template-columns:repeat(2,1fr)}}
</style>
<div class='nav'>
<a href='/'>Dashboard</a>
<a href='/history'>Historie</a>
<a class='active' href='/settings'>Einstellungen</a>
<a href='/systemstatus'>System</a>
</div>
<form method='POST' action='/config/save'>
<div class='settingsGrid'>

<div class='settingsBlock'>
<h2>🌐 WLAN</h2>
<p>Verbindung zum vorhandenen WLAN. Änderungen werden nach dem Speichern übernommen.</p>
<label>SSID</label>
<input name='wifiSSID' maxlength='32' value=')HTML";
  html += htmlEscape(cfg.wifiSSID);
  html += R"HTML('>
<label>Passwort</label>
<input type='password' name='wifiPassword' maxlength='64' value=')HTML";
  html += htmlEscape(cfg.wifiPassword);
  html += R"HTML('>
</div>

<div class='settingsBlock'>
<h2>📡 MQTT</h2>
<p>Verbindung zum MQTT-Broker. Standard-MQTT-Intervall: 3 Minuten.</p>
<label><input type='checkbox' name='mqttEnabled' value='1' )HTML";
  if (mqttEnabled) html += "checked";
  html += R"HTML(> MQTT aktivieren</label>
<span class='settingsHelp'>Deaktiviert MQTT vollständig; Messung, Historie und Weboberfläche laufen weiter.</span>
<label>Broker / Host</label>
<input name='mqttHost' maxlength='64' value=')HTML";
  html += htmlEscape(cfg.mqttHost);
  html += R"HTML('>
<label>Port</label>
<input type='number' min='1' max='65535' name='mqttPort' value=')HTML";
  html += String(cfg.mqttPort);
  html += R"HTML('>
<label>Benutzer</label>
<input name='mqttUser' maxlength='32' value=')HTML";
  html += htmlEscape(cfg.mqttUser);
  html += R"HTML('>
<label>Passwort</label>
<input type='password' name='mqttPassword' maxlength='64' value=')HTML";
  html += htmlEscape(cfg.mqttPassword);
  html += R"HTML('>
</div>

<div class='settingsBlock'>
<h2>📨 MQTT-Topics</h2>
<p>Topics für Messwerte, Home Assistant und bestehende MQTT-Auswertungen.</p>
<label>Basis-Topic</label>
<input name='mqttTopic' maxlength='64' value=')HTML";
  html += htmlEscape(cfg.mqttTopic);
  html += R"HTML('>
<label>Kompatibilitäts-Topic: average</label>
<input name='mqttAverageTopic' maxlength='64' value=')HTML";
  html += htmlEscape(cfg.mqttAverageTopic);
  html += R"HTML('>
<span class='settingsHelp'>Aktueller Tankinhalt in Litern</span>
<label>Kompatibilitäts-Topic: fuellhoehe</label>
<input name='mqttFuellhoeheTopic' maxlength='64' value=')HTML";
  html += htmlEscape(cfg.mqttFuellhoeheTopic);
  html += R"HTML('>
<span class='settingsHelp'>Aktueller Sensorabstand in mm</span>
</div>

<div class='settingsBlock'>
<h2>⏱ Messung & MQTT-Intervall</h2>
<p>Hier werden die Zeitabstände in verständlichen Einheiten eingestellt.</p>
<label>Messintervall (Sekunden)</label>
<input type='number' min='1' max='600' name='measurementIntervalSec' value=')HTML";
  html += String(cfg.measurementInterval / 1000UL);
  html += R"HTML('>
<span class='settingsHelp'>Aktueller Standard: 20 Sekunden</span>
<label>MQTT-Intervall (Minuten)</label>
<input type='number' min='1' max='60' name='mqttIntervalMin' value=')HTML";
  html += String(cfg.mqttInterval / 60000UL);
  html += R"HTML('>
<span class='settingsHelp'>Aktueller Standard: 3 Minuten</span>
</div>

<div class='settingsBlock'>
<h2>🕒 Uhrzeit / NTP</h2>
<p>Zeitsynchronisation für Datum, Uhrzeit und Historie.</p>
<label>NTP aktiv</label>
<label><input type='checkbox' name='ntpEnabled')HTML";
  if (cfg.ntpEnabled) html += " checked";
  html += R"HTML(>NTP verwenden</label>
<label>NTP-Server</label>
<input name='ntpServer' maxlength='64' value=')HTML";
  html += htmlEscape(cfg.ntpServer);
  html += R"HTML('>
<label>Zeitzone UTC-Offset (Stunden)</label>
<input type='number' step='0.25' min='-12' max='14' name='timezoneHours' value=')HTML";
  html += String((float)cfg.timezoneOffset / 3600.0f, 2);
  html += R"HTML('>
<span class='settingsHelp'>Deutschland: im Winter UTC+1, im Sommer UTC+2. Die NTP-Sommerzeitlogik kann später erweitert werden.</span>
</div>

<div class='settingsBlock'>
<h2>⬆ Arduino OTA</h2>
<p>Passwortschutz für Firmware-Uploads über den Netzwerk-Port der Arduino IDE.</p>
<label>OTA-Passwort</label>
<input type='password' name='otaPassword' maxlength='63' value=')HTML";
  html += htmlEscape(otaPassword);
  html += R"HTML(' autocomplete='new-password'>
<span class='settingsHelp'>Leer lassen = kein ArduinoOTA-Passwort. Nach einer Änderung wird ein Neustart empfohlen.</span>
</div>

<div class='settingsBlock'>
<h2>🛢 Tank</h2>
<p>Geometrie und Abmessungen für die Literberechnung.</p>
<label>Geometrie</label>
<select name='geometry'>
<option value='0')HTML";
  if (cfg.geometry == GEOMETRY_CYLINDER) html += " selected";
  html += R"HTML(>Zylinder</option>
<option value='1')HTML";
  if (cfg.geometry == GEOMETRY_BOX) html += " selected";
  html += R"HTML(>Quader</option>
</select>
<label>Durchmesser (mm)</label>
<input type='number' step='0.1' name='diameter' value=')HTML";
  html += String(cfg.diameter, 1);
  html += R"HTML('>
<label>Länge (mm)</label>
<input type='number' step='0.1' name='tankLength' value=')HTML";
  html += String(cfg.tankLength, 1);
  html += R"HTML('>
<label>Breite (mm)</label>
<input type='number' step='0.1' name='tankWidth' value=')HTML";
  html += String(cfg.tankWidth, 1);
  html += R"HTML('>
<label>Tankhöhe (mm)</label>
<input type='number' step='0.1' name='tankHeight' value=')HTML";
  html += String(cfg.tankHeight, 1);
  html += R"HTML('>
</div>

<div class='settingsBlock'>
<h2>📏 Füllstand / Kalibrierung</h2>
<p>Referenzwerte für die Umrechnung des Sensorabstands in Füllstand und Liter.</p>
<label>Leer-Distanz (mm)</label>
<input type='number' step='0.1' name='emptyDistance' value=')HTML";
  html += String(cfg.emptyDistance, 1);
  html += R"HTML('>
<label>Voll-Distanz (mm)</label>
<input type='number' step='0.1' name='fullDistance' value=')HTML";
  html += String(cfg.fullDistance, 1);
  html += R"HTML('>
<label>Sensor Offset (mm)</label>
<input type='number' name='sensorOffset' value=')HTML";
  html += String(cfg.sensorOffset);
  html += R"HTML('>
<label>LOW-Grenze (%)</label>
<input type='number' step='0.1' min='0' max='100' name='lowLevelPercent' value=')HTML";
  html += String(cfg.lowLevelPercent, 1);
  html += R"HTML('>
<label>HIGH-Grenze (%)</label>
<input type='number' step='0.1' min='0' max='100' name='highLevelPercent' value=')HTML";
  html += String(cfg.highLevelPercent, 1);
  html += R"HTML('>
<div class='warnBox'>Für eine normale Kalibrierung besser den Kalibrier-Assistenten auf der Systemseite verwenden.</div>
</div>

<div class='settingsBlock'>
<h2>📡 ToF-Sensor / I²C</h2>
<p>Erkennung, Sensorwahl und I²C-Parameter. Die Hardware ist auf die V3.1-Zuordnung ausgelegt.</p>
<label>ToF-Sensor</label>
<select name='vlSensorType'>
<option value='255')HTML";
  if (cfg.vlSensorType == SENSOR_AUTO) html += " selected";
  html += R"HTML(>Automatisch erkennen</option>
<option value='0')HTML";
  if (cfg.vlSensorType == SENSOR_VL53L0X) html += " selected";
  html += R"HTML(>VL53L0X</option>
<option value='1')HTML";
  if (cfg.vlSensorType == SENSOR_VL53L1X) html += " selected";
  html += R"HTML(>VL53L1X</option>
<option value='2')HTML";
  if (cfg.vlSensorType == SENSOR_VL53L5CX) html += " selected";
  html += R"HTML(>VL53L5CX</option>
</select>
<span class='settingsHelp'>Erkannt aktuell: )HTML";
  html += htmlEscape(sensorName());
  html += R"HTML(</span>
<label>VL53 I²C-Adresse</label>
<input type='text' name='vlAddress' maxlength='4' pattern='0[xX][0-9A-Fa-f]{1,2}' value=')HTML";
  html += formatI2CAddress(cfg.vlAddress);
  html += R"HTML('>
<label>BME280 I²C-Adresse</label>
<input type='text' name='bmeAddress' maxlength='4' pattern='0[xX][0-9A-Fa-f]{1,2}' value=')HTML";
  html += formatI2CAddress(cfg.bmeAddress);
  html += R"HTML('>
<label>SDA Pin</label>
<input type='number' min='0' max='21' name='sdaPin' value=')HTML";
  html += String(cfg.sdaPin);
  html += R"HTML('>
<label>SCL Pin</label>
<input type='number' min='0' max='21' name='sclPin' value=')HTML";
  html += String(cfg.sclPin);
  html += R"HTML('>
<label>OLED I²C-Adresse</label>
<input type='text' name='oledAddress' maxlength='4' pattern='0[xX][0-9A-Fa-f]{1,2}' value=')HTML";
  html += formatI2CAddress(cfg.oledAddress);
  html += R"HTML('>
</div>

<div class='settingsBlock'>
<h2>🎛 Filter / Messqualität</h2>
<p>Glättung und Plausibilitätsprüfung der ToF-Messwerte.</p>
<label>Moving-Average Samples</label>
<input type='number' min='1' max='64' name='sampleCount' value=')HTML";
  html += String(cfg.sampleCount);
  html += R"HTML('>
<label>Medianfilter aktiv</label>
<label><input type='checkbox' name='medianEnabled')HTML";
  if (cfg.medianEnabled) html += " checked";
  html += R"HTML(>Medianfilter verwenden</label>
<label>Median Samples</label>
<input type='number' min='3' max='7' step='2' name='medianSamples' value=')HTML";
  html += String(cfg.medianSamples);
  html += R"HTML('>
<label>Min. Entfernung (mm)</label>
<input type='number' min='1' name='minDistance' value=')HTML";
  html += String(cfg.minDistance);
  html += R"HTML('>
<label>Max. Entfernung (mm)</label>
<input type='number' min='1' name='maxDistance' value=')HTML";
  html += String(cfg.maxDistance);
  html += R"HTML('>
<label>Max. erlaubter Sprung (mm)</label>
<input type='number' min='1' name='maxJump' value=')HTML";
  html += String(cfg.maxJump);
  html += R"HTML('>
</div>

<div class='settingsBlock'>
<h2>🖥 OLED</h2>
<p>Anzeige und automatischer Seitenwechsel.</p>
<label>OLED aktiv</label>
<label><input type='checkbox' name='oledEnabled')HTML";
  if (cfg.oledEnabled) html += " checked";
  html += R"HTML(>OLED verwenden</label>
<label>Seitenintervall (Sekunden)</label>
<input type='number' min='1' max='3600' name='oledPageIntervalSec' value=')HTML";
  html += String(cfg.oledPageInterval / 1000UL);
  html += R"HTML('>
<label>Display-Taste GPIO</label>
<input type='number' min='0' max='48' name='displayButtonPin' value=')HTML";
  html += String(displayButtonPin);
  html += R"HTML('>
<span class='settingsHelp'>ESP32-C3 Super Mini: BOOT-Taste = GPIO 9. Bei anderen ESP32-Varianten an die verwendete Taste anpassen.</span>
</div>

<div class='settingsBlock'>
<h2>🧪 Diagnose</h2>
<p>Ausführlichkeit der seriellen Debugmeldungen.</p>
<label>Debug-Level</label>
<select name='debugLevel'>
)HTML";
  const char* debugLabels[] = {
    "0 - AUS", "1 - FEHLER", "2 - WARNUNGEN", "3 - NORMAL", "4 - DETAILLIERT", "5 - SENSOR / KOMMUNIKATION"
  };
  for (uint8_t level = 0; level <= DEBUG_LEVEL_SENSOR; level++) {
    html += "<option value='" + String(level) + "'";
    if (cfg.debugLevel == level) html += " selected";
    html += ">" + String(debugLabels[level]) + "</option>";
  }
  html += R"HTML(
</select>
<span class='settingsHelp'>Regelmäßige Statusmeldungen bleiben auf 10 Minuten begrenzt.</span>
</div>

<div class='settingsBlock'>
<h2>🔧 Erweiterte Hinweise</h2>
<p>Die folgenden Werte sind absichtlich hier gebündelt, damit alle wichtigen Parameter auf einer Seite erreichbar sind.</p>
<div class='metric-card'><b>Aktueller Sensor</b><div>)HTML";
  html += htmlEscape(sensorName());
  html += R"HTML(</div></div>
<div class='metric-card' style='margin-top:8px'><b>Firmware</b><div>)HTML";
  html += FW_VERSION;
  html += R"HTML(</div></div>
<div class='warnBox'>Änderungen an I²C-Pins, I²C-Adressen oder Sensorwahl können die Hardwarekommunikation beeinflussen. Bei Unsicherheit „Automatisch erkennen“ verwenden.</div>
</div>

</div>
<div class='settingsActions'><button class='saveBtn' type='submit'>💾 Einstellungen speichern</button></div>
</form>
)HTML";

  html += pageFooter();
  sendLocalizedHtml(200, html);
}

void handleConfig() {
  // Kompatibilitätsroute: /config zeigt jetzt dieselbe zentrale Einstellungsseite.
  handleSettings();
}

// Kopiert einen HTTP-String sicher in ein char-Array.
// Verhindert Überläufe und garantiert die abschließende NUL-Zeichenkette.
static void copyArg(const char* name, char* dest, size_t destSize) {
  if (!server || !name || !dest || destSize == 0) return;
  if (!server->hasArg(name)) return;

  String value = server->arg(name);
  value.trim();

  size_t n = value.length();
  if (n >= destSize) n = destSize - 1;

  memcpy(dest, value.c_str(), n);
  dest[n] = '\0';
}

void handleConfigSave() {

  copyArg("wifiSSID", cfg.wifiSSID, sizeof(cfg.wifiSSID));
  copyArg("wifiPassword", cfg.wifiPassword, sizeof(cfg.wifiPassword));

  const bool mqttWasEnabled = mqttEnabled;
  mqttEnabled = server->hasArg("mqttEnabled");
  saveMqttEnabled();

  if (mqttWasEnabled && !mqttEnabled) {
    if (mqttClient.connected()) mqttClient.disconnect();
    mqttServerConfigured = false;
    lastMqttAttempt = 0;
  } else if (!mqttWasEnabled && mqttEnabled) {
    mqttServerConfigured = false;
    lastMqttAttempt = 0;
    mqttReconnectDelay = MQTT_RECONNECT_INTERVAL;
  }

  copyArg("mqttHost", cfg.mqttHost, sizeof(cfg.mqttHost));
  copyArg("mqttUser", cfg.mqttUser, sizeof(cfg.mqttUser));
  copyArg("mqttPassword", cfg.mqttPassword, sizeof(cfg.mqttPassword));
  copyArg("mqttTopic", cfg.mqttTopic, sizeof(cfg.mqttTopic));
  copyArg("mqttAverageTopic", cfg.mqttAverageTopic, sizeof(cfg.mqttAverageTopic));
  copyArg("mqttFuellhoeheTopic", cfg.mqttFuellhoeheTopic, sizeof(cfg.mqttFuellhoeheTopic));
  copyArg("ntpServer", cfg.ntpServer, sizeof(cfg.ntpServer));

  if (server->hasArg("mqttPort")) cfg.mqttPort = (uint16_t)server->arg("mqttPort").toInt();
  if (server->hasArg("timezoneHours")) {
    float hours = server->arg("timezoneHours").toFloat();
    cfg.timezoneOffset = (int32_t)roundf(hours * 3600.0f);
  }
  cfg.ntpEnabled = server->hasArg("ntpEnabled");

  if (server->hasArg("otaPassword")) {
    String newOtaPassword = server->arg("otaPassword");
    newOtaPassword.trim();
    if (newOtaPassword.length() > 63) {
      newOtaPassword.remove(63);
    }
    if (newOtaPassword != otaPassword) {
      otaPassword = newOtaPassword;
      saveOtaPassword();
    }
  }

  if (server->hasArg("vlSensorType")) cfg.vlSensorType = (uint8_t)server->arg("vlSensorType").toInt();

  if (server->hasArg("vlAddress")) {
    uint8_t parsedAddress;
    if (parseI2CAddress(server->arg("vlAddress"), parsedAddress)) cfg.vlAddress = parsedAddress;
  }
  if (server->hasArg("bmeAddress")) {
    uint8_t parsedAddress;
    if (parseI2CAddress(server->arg("bmeAddress"), parsedAddress)) cfg.bmeAddress = parsedAddress;
  }
  if (server->hasArg("oledAddress")) {
    uint8_t parsedAddress;
    if (parseI2CAddress(server->arg("oledAddress"), parsedAddress)) cfg.oledAddress = parsedAddress;
  }

  if (server->hasArg("sdaPin")) cfg.sdaPin = (uint8_t)server->arg("sdaPin").toInt();
  if (server->hasArg("sclPin")) cfg.sclPin = (uint8_t)server->arg("sclPin").toInt();

  if (server->hasArg("sampleCount")) cfg.sampleCount = (uint8_t)server->arg("sampleCount").toInt();
  cfg.medianEnabled = server->hasArg("medianEnabled");
  if (server->hasArg("medianSamples")) cfg.medianSamples = (uint8_t)server->arg("medianSamples").toInt();

  if (server->hasArg("minDistance")) cfg.minDistance = (uint16_t)server->arg("minDistance").toInt();
  if (server->hasArg("maxDistance")) cfg.maxDistance = (uint16_t)server->arg("maxDistance").toInt();
  if (server->hasArg("maxJump")) cfg.maxJump = (uint16_t)server->arg("maxJump").toInt();
  if (server->hasArg("sensorOffset")) cfg.sensorOffset = (int16_t)server->arg("sensorOffset").toInt();

  if (server->hasArg("geometry")) cfg.geometry = (uint8_t)server->arg("geometry").toInt();
  if (server->hasArg("diameter")) cfg.diameter = server->arg("diameter").toFloat();
  if (server->hasArg("tankLength")) cfg.tankLength = server->arg("tankLength").toFloat();
  if (server->hasArg("tankWidth")) cfg.tankWidth = server->arg("tankWidth").toFloat();
  if (server->hasArg("tankHeight")) cfg.tankHeight = server->arg("tankHeight").toFloat();
  if (server->hasArg("emptyDistance")) cfg.emptyDistance = server->arg("emptyDistance").toFloat();
  if (server->hasArg("fullDistance")) cfg.fullDistance = server->arg("fullDistance").toFloat();
  if (server->hasArg("lowLevelPercent")) cfg.lowLevelPercent = server->arg("lowLevelPercent").toFloat();
  if (server->hasArg("highLevelPercent")) cfg.highLevelPercent = server->arg("highLevelPercent").toFloat();

  if (server->hasArg("measurementIntervalSec")) {
    uint32_t sec = (uint32_t)server->arg("measurementIntervalSec").toInt();
    cfg.measurementInterval = sec * 1000UL;
  }
  if (server->hasArg("mqttIntervalMin")) {
    uint32_t min = (uint32_t)server->arg("mqttIntervalMin").toInt();
    cfg.mqttInterval = min * 60000UL;
  }

  cfg.oledEnabled = server->hasArg("oledEnabled");
  if (server->hasArg("oledPageIntervalSec")) {
    uint32_t sec = (uint32_t)server->arg("oledPageIntervalSec").toInt();
    cfg.oledPageInterval = sec * 1000UL;
  }

  if (server->hasArg("displayButtonPin")) {
    long pin = server->arg("displayButtonPin").toInt();
    if (pin >= 0 && pin <= 48) {
      displayButtonPin = (uint8_t)pin;
      saveDisplayButtonPin();
      pinMode(displayButtonPin, INPUT_PULLUP);
    }
  }

  if (server->hasArg("debugLevel")) cfg.debugLevel = (uint8_t)server->arg("debugLevel").toInt();

  validateConfig();
  saveConfig();
  mqttClient.disconnect();

  String html = pageHeader("Einstellungen gespeichert");
  html += R"HTML(
<div class='nav'><a href='/'>Dashboard</a><a href='/history'>Historie</a><a class='active' href='/settings'>Einstellungen</a><a href='/systemstatus'>System</a></div>
<div class='card'>
<h1>✓ Einstellungen gespeichert</h1>
<p>Die Konfiguration wurde erfolgreich gespeichert.</p>
<p>Bei Änderungen an WLAN, OTA-Passwort, I²C, Sensorwahl oder anderen Hardwareparametern wird ein Neustart empfohlen.</p>
<a class='btn' href='/settings'>Zurück zu Einstellungen</a>
<a class='btn' href='/reboot'>🔄 Neustart</a>
</div>
)HTML";
  html += pageFooter();
  sendLocalizedHtml(200, html);
}

// ============================================================================
// WEB KALIBRIERUNG
// ============================================================================

void handleCalibration() {

  String html = pageHeader("Kalibrierung");

  html += "<div class='card'>";
  html += "<h1>Kalibrier-Assistent</h1>";
  html += "<p>Hier kann ein bekannter aktueller Füllstand als Referenz verwendet werden. Der ESP32 nimmt den aktuell gemessenen Sensorabstand und berechnet daraus die passende Flüssigkeitshöhe.</p>";

  html += "<table>";
  html += "<tr><th>Sensor</th><td>" + htmlEscape(sensorName()) + "</td></tr>";
  html += "<tr><th>Aktueller Sensorabstand</th><td>";
  html += isfinite(filteredDistance) ? String(filteredDistance, 1) : "--";
  html += " mm</td></tr>";
  html += "<tr><th>Aktuell berechneter Füllstand</th><td>";
  html += isfinite(tankLiters) ? String(tankLiters, 1) : "--";
  html += " L</td></tr>";
  html += "<tr><th>Tankhöhe</th><td>" + String(cfg.tankHeight, 1) + " mm</td></tr>";
  html += "<tr><th>Leer-Distanz</th><td>" + String(cfg.emptyDistance, 1) + " mm</td></tr>";
  html += "<tr><th>Voll-Distanz</th><td>" + String(cfg.fullDistance, 1) + " mm</td></tr>";
  html += "</table>";
  html += "</div>";

  html += "<div class='card'>";
  html += "<h2>Bekannter Füllstand</h2>";
  html += "<form method='POST' action='/calibration/apply'>";
  html += "<label>Aktueller realer Füllstand in Liter</label>";
  html += "<input type='number' step='0.1' min='0' name='calibrationLiters' required>";
  html += "<button type='submit'>Kalibrierung berechnen und übernehmen</button>";
  html += "</form>";
  html += "<p>Die Messspanne zwischen Leer und Voll bleibt erhalten. Es wird der Nullpunkt anhand des bekannten Volumens korrigiert.</p>";
  html += "</div>";

  html += "<div class='card'>";
  html += "<h2>Direkte Referenzpunkte</h2>";
  html += "<p>Wenn der Tank nachweislich leer oder voll ist, kann der aktuelle Sensorabstand direkt als Referenz gespeichert werden.</p>";
  html += "<form method='POST' action='/calibration/apply'>";
  html += "<input type='hidden' name='referencePoint' value='empty'>";
  html += "<button type='submit'>Aktuellen Abstand als LEER speichern</button>";
  html += "</form>";
  html += "<form method='POST' action='/calibration/apply'>";
  html += "<input type='hidden' name='referencePoint' value='full'>";
  html += "<button type='submit'>Aktuellen Abstand als VOLL speichern</button>";
  html += "</form>";
  html += "</div>";

  html += pageFooter();
  sendLocalizedHtml(200, html);
}

void handleCalibrationApply() {

  if (!measurementValid || !isfinite(filteredDistance)) {
    server->send(409, "text/plain", "Keine gültige aktuelle Sensor-Messung verfügbar.");
    return;
  }

  // Direkte Leer-/Vollreferenz
  if (server->hasArg("referencePoint")) {
    String point = server->arg("referencePoint");

    if (point == "empty") {
      if (filteredDistance <= cfg.fullDistance + 1.0f) {
        server->send(400, "text/plain", "Leer-Referenz ist nicht plausibel: Sensorabstand muss größer als Voll-Distanz sein.");
        return;
      }
      cfg.emptyDistance = filteredDistance;
    } else if (point == "full") {
      if (filteredDistance >= cfg.emptyDistance - 1.0f) {
        server->send(400, "text/plain", "Voll-Referenz ist nicht plausibel: Sensorabstand muss kleiner als Leer-Distanz sein.");
        return;
      }
      cfg.fullDistance = filteredDistance;
    } else {
      server->send(400, "text/plain", "Unbekannter Referenzpunkt.");
      return;
    }

    validateConfig();
    saveConfig();
    resetFilters();

    String html = pageHeader("Kalibrierung gespeichert");
    html += "<div class='card'><h1>Kalibrierung gespeichert</h1>";
    html += "<p>Leer-Distanz: " + String(cfg.emptyDistance, 1) + " mm</p>";
    html += "<p>Voll-Distanz: " + String(cfg.fullDistance, 1) + " mm</p>";
    html += "<a class='btn' href='/calibration'>Zurück</a></div>";
    html += pageFooter();
    sendLocalizedHtml(200, html);
    return;
  }

  if (!server->hasArg("calibrationLiters")) {
    server->send(400, "text/plain", "Kein Liter-Referenzwert angegeben.");
    return;
  }

  float liters = server->arg("calibrationLiters").toFloat();
  float heightMm = NAN;
  float newEmpty = NAN;
  float newFull = NAN;

  if (!calibrateFromKnownLiters(liters, filteredDistance, newEmpty, newFull, heightMm)) {
    server->send(400, "text/plain", "Literwert außerhalb des konfigurierten Tankvolumens oder Kalibrierung ungültig.");
    return;
  }

  cfg.emptyDistance = newEmpty;
  cfg.fullDistance = newFull;

  validateConfig();
  saveConfig();
  resetFilters();
  calculateTank(filteredDistance);

  float theoreticalDistance = NAN;
  float theoreticalHeight = NAN;
  calculateDistanceFromLiters(liters, theoreticalDistance, theoreticalHeight);

  String html = pageHeader("Kalibrierung gespeichert");
  html += "<div class='card'><h1>Liter-Kalibrierung gespeichert</h1>";
  html += "<table>";
  html += "<tr><th>Referenzvolumen</th><td>" + String(liters, 1) + " L</td></tr>";
  html += "<tr><th>Berechnete Flüssigkeitshöhe</th><td>" + String(heightMm, 1) + " mm</td></tr>";
  html += "<tr><th>Gemessener Sensorabstand</th><td>" + String(filteredDistance, 1) + " mm</td></tr>";
  html += "<tr><th>Neue Leer-Distanz</th><td>" + String(cfg.emptyDistance, 1) + " mm</td></tr>";
  html += "<tr><th>Neue Voll-Distanz</th><td>" + String(cfg.fullDistance, 1) + " mm</td></tr>";
  html += "<tr><th>Theoretischer Sensorabstand</th><td>" + String(theoreticalDistance, 1) + " mm</td></tr>";
  html += "</table>";
  html += "<p>Die Kalibrierspanne wurde beibehalten und der Nullpunkt auf den bekannten aktuellen Füllstand verschoben.</p>";
  html += "<a class='btn' href='/'>Status</a><a class='btn' href='/calibration'>Kalibrierung</a></div>";
  html += pageFooter();
  sendLocalizedHtml(200, html);
}

// ============================================================================
// WEB STATUS
// ============================================================================

void handleStatus() {
  String html = pageHeader("Systemstatus");

  html += "<div class='card'>";
  html += "<div class='topbar'>";
  html += "<h1>Systemstatus</h1>";
  html += "<div class='links'><a class='btn' href='/'>← Dashboard</a><a class='btn' href='/api/status'>JSON</a></div>";
  html += "</div>";
  html += "<p>Aktueller Gerätestatus als Weboberfläche.</p>";
  html += "<table>";
  html += "<tr><th>Firmware</th><td>" + String(FW_VERSION) + "</td></tr>";
  html += "<tr><th>WLAN</th><td>" + String(WiFi.status() == WL_CONNECTED ? "verbunden" : "getrennt") + "</td></tr>";
  html += "<tr><th>MQTT</th><td>" + String(mqttClient.connected() ? "verbunden" : "getrennt") + "</td></tr>";
  html += "<tr><th>Füllstand</th><td>" + String(tankPercent, 1) + " %</td></tr>";
  html += "<tr><th>Liter</th><td>" + String(tankLiters, 1) + " L</td></tr>";
  html += "<tr><th>Temperatur</th><td>" + String(temperature, 1) + " °C</td></tr>";
  html += "<tr><th>Feuchte</th><td>" + String(humidity, 1) + " %</td></tr>";
  html += "<tr><th>Taupunkt</th><td>" + String(dewPoint, 1) + " °C</td></tr>";
  {
    time_t now = time(nullptr);
    struct tm timeinfo;
    char dateBuffer[16];
    char timeBuffer[16];

    if (now > 0 && localtime_r(&now, &timeinfo) != nullptr) {
      strftime(dateBuffer, sizeof(dateBuffer), "%d.%m.%Y", &timeinfo);
      strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeinfo);
    } else {
      strlcpy(dateBuffer, "--.--.----", sizeof(dateBuffer));
      strlcpy(timeBuffer, "--:--:--", sizeof(timeBuffer));
    }

    html += "<tr><th>Datum</th><td>" + String(dateBuffer) + "</td></tr>";
    html += "<tr><th>Uhrzeit</th><td>" + String(timeBuffer) + "</td></tr>";
  }
  html += "</table>";
  html += "</div>";
  html += pageFooter();
  sendLocalizedHtml(200, html);
}

void handleApiPing() {
  String json;
  json.reserve(256);

  json += "{";
  json += "\"ok\":true,";
  json += "\"version\":\"";
  json += jsonEscape(FW_VERSION);
  json += "\",";
  json += "\"uptime_ms\":";
  json += String(millis());
  json += ",";
  json += "\"wifi_connected\":";
  json += (WiFi.status() == WL_CONNECTED ? "true" : "false");
  json += ",";
  json += "\"ip\":\"";
  json += jsonEscape(WiFi.localIP().toString());
  json += "\",";
  json += "\"rssi\":";
  json += String(WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0);
  json += ",";
  json += "\"ntp_synced\":";
  json += (time(nullptr) >= 1000000000UL ? "true" : "false");
  json += "}";

  server->send(200, "application/json", json);
}

void handleStatusJson() {
  String json = buildMqttJson();
  server->sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server->sendHeader("Pragma", "no-cache");
  server->send(200, "application/json; charset=utf-8", json);
}

void handleSystemStatusPage() {
  String html=pageHeader("System / Wartung");html.reserve(9000);
  html += "<div class='nav'><a href='/'>Dashboard</a><a href='/history'>Historie</a><a href='/settings'>Einstellungen</a><a class='active' href='/systemstatus'>System</a></div>";
  html += "<div class='card'><h1>System / Wartung</h1><div class='grid'>";
  html += "<div class='metric-card'><h3>Firmware</h3><div>"+String(FW_VERSION)+"</div></div>";
  html += "<div class='metric-card'><h3>Gerät</h3><div>ESP32-C3 Super Mini</div></div>";
  html += "<div class='metric-card'><h3>ESP-Temperatur</h3><div>"+String(temperatureRead(),1)+" °C</div></div>";
  html += "<div class='metric-card'><h3>Uptime</h3><div>"+String(millis()/1000UL)+" s</div></div>";
  const uint32_t heapFreeNow = ESP.getFreeHeap();
  const uint32_t heapMinSeen = ESP.getMinFreeHeap();
  const uint32_t heapLargest = ESP.getMaxAllocHeap();
  const char* heapState = heapHealthText(heapMinSeen, heapLargest);

  html += "<div class='metric-card'><h3>System-Health</h3><div>"+String(overallHealthText())+"</div><small>"+overallHealthDetails()+"</small></div>";
  html += "<div class='metric-card'><h3>Letzte gueltige Messung</h3><div>";
  if (lastValidMeasurementMs == 0) html += "noch keine";
  else html += String((millis()-lastValidMeasurementMs)/1000UL) + " s";
  html += "</div><small>Zeit seit letzter erfolgreicher ToF-Messung</small></div>";
  html += "<div class='metric-card'><h3>Heap frei</h3><div>"+String(heapFreeNow/1024.0f,1)+" KB</div></div>";
  html += "<div class='metric-card'><h3>Heap Minimum</h3><div>"+String(heapMinSeen/1024.0f,1)+" KB</div></div>";
  html += "<div class='metric-card'><h3>Largest Block</h3><div>"+String(heapLargest/1024.0f,1)+" KB</div></div>";
  html += "<div class='metric-card'><h3>Kleinster Largest Block</h3><div>"+String((lowestLargestBlockSinceBoot==0xFFFFFFFFUL?heapLargest:lowestLargestBlockSinceBoot)/1024.0f,1)+" KB</div><small>Seit diesem Boot</small></div>";
  html += "<div class='metric-card'><h3>Heap-Status</h3><div>"+String(heapState)+"</div><small>OK ab 20 KB, Warnung 19-20 KB, kritisch &lt;19 KB Minimum-Heap</small></div>";
  html += "<div class='metric-card'><h3>Letzter Neustart</h3><div>"+String(resetReasonText(bootResetReason))+"</div><small>Reset-Code "+String((int)bootResetReason)+(resetReasonIsCritical(bootResetReason) ? " · kritisch" : "")+"</small></div>";
  html += "<div class='metric-card'><h3>Neustarts gesamt</h3><div>"+String(persistentRestartCount)+"</div><small>Persistent gespeichert</small></div>";
  html += "<div class='metric-card'><h3>Sensor-Reinitialisierungen</h3><div>"+String(sensorReinitCount)+"</div><small>Seit Boot</small></div>";
  html += "<div class='metric-card'><h3>WLAN-Reconnects</h3><div>"+String(wifiReconnectCountSinceBoot)+"</div><small>Seit Boot, Erstverbindung nicht gezaehlt</small></div>";
  html += "<div class='metric-card'><h3>MQTT-Reconnects</h3><div>"+String(mqttReconnectCountSinceBoot)+"</div><small>Seit Boot, Erstverbindung nicht gezaehlt</small></div>";
  html += "<div class='metric-card'><h3>Display-Taste</h3><div>GPIO "+String(displayButtonPin)+"</div></div>";
  html += "<div class='metric-card'><h3>Flash</h3><div>"+String(ESP.getFlashChipSize()/1024.0f/1024.0f,1)+" MB</div></div>";
  html += "<div class='metric-card'><h3>Sketch</h3><div>"+String(ESP.getSketchSize()/1024.0f,1)+" KB</div></div>";
  html += "<div class='metric-card'><h3>Sketch frei</h3><div>"+String(ESP.getFreeSketchSpace()/1024.0f,1)+" KB</div></div>";
  html += "<div class='metric-card'><h3>SPIFFS</h3><div>"+String(SPIFFS.totalBytes()/1024.0f,1)+" / "+String(SPIFFS.usedBytes()/1024.0f,1)+" KB</div></div>";
  html += "<div class='metric-card'><h3>WLAN</h3><div>"; html += (WiFi.status()==WL_CONNECTED ? "Verbunden" : "Fehler"); html += "</div></div>";
  html += "<div class='metric-card'><h3>MQTT</h3><div>"; html += (mqttClient.connected() ? "Verbunden" : "Offline"); html += "</div></div>";
  html += "<div class='metric-card'><h3>Home Assistant</h3><div>MQTT Discovery</div><small>Discovery-Prefix: homeassistant</small></div>";
  html += "<div class='metric-card'><h3>ToF</h3><div>"; html += String(sensorName()); html += " / "; html += (vlOK ? "OK" : "Fehler"); html += "</div></div>";
  html += "<div class='metric-card'><h3>BME280</h3><div>"; html += (bmeOK ? "OK" : "Fehler"); html += "</div></div>";
  html += "<div class='metric-card'><h3>Historie</h3><div>10 Jahre / 3650 Tage</div></div>";
  html += "<div class='metric-card'><h3>OTA</h3><div>Bereit</div></div>";
  html += "</div></div>";
  html += "<div class='card'><h2>Testdaten</h2><p class='muted'>Erzeugt realistische Testhistorien mit saisonalem Verbrauch und Nachfüllvorgängen.</p><p>Quelle: <select id='testSource'><option value='test'>TEST</option><option value='measured'>GEMESSEN</option><option value='imported'>IMPORTIERT</option></select></p><div class='linkrow'><form method='POST' action='/generate-test-history' style='display:inline' onsubmit=\"this.insertAdjacentHTML('beforeend','<input type=hidden name=source value='+document.getElementById('testSource').value+'>');return confirm('1 Jahr Testdaten erzeugen? Bestehende Historie wird ersetzt.')\"><button class='btn' type='submit'>🧪 1 Jahr Testdaten</button></form><form method='POST' action='/generate-test-history-10y' style='display:inline' onsubmit=\"this.insertAdjacentHTML('beforeend','<input type=hidden name=source value='+document.getElementById('testSource').value+'>');return confirm('10 Jahre Testdaten erzeugen? Bestehende Historie wird ersetzt.')\"><button class='btn' type='submit'>🧪 10 Jahre Testdaten</button></form><form method='POST' action='/clear-consumption-data' style='display:inline' onsubmit=\"return confirm('Alle Verbrauchs- und Historiedaten wirklich löschen?')\"><button class='btn danger' type='submit'>🗑 Verbrauchsdaten löschen</button></form></div></div>";
  html += "<div class='card'><h2>Wartungsaktionen</h2><div class='linkrow'><a class='btn' href='/calibration'>🔧 Kalibrierung</a><a class='btn' href='/update'>⬆ OTA / Firmware</a><a class='btn' href='/reboot' onclick=\"return confirm('ESP32 wirklich neu starten?')\">🔄 Reboot</a><a class='btn danger' href='/factory-reset' onclick=\"return confirm('Werkseinstellungen wirklich zurücksetzen?')\">⚠ Factory Reset</a></div></div>";
  html += pageFooter();sendLocalizedHtml(200, html);
}

// ===== KOMPATIBILITÄT: ältere Tankverbrauch-/Nachfülllogik =====
#define V5_4_1_CONSUMPTION 1





static uint32_t historyCrc32Update(uint32_t crc,const uint8_t*data,size_t len){
  while(len--){crc^=*data++;for(uint8_t i=0;i<8;i++)crc=(crc>>1)^(0xEDB88320UL&(-(int32_t)(crc&1U)));}
  return crc;
}
static uint32_t historyCrc32(const uint8_t*data,size_t len){return ~historyCrc32Update(0xFFFFFFFFUL,data,len);}
static bool historyEnsureSpiffs(){
  if(historySpiffsReady)return true;

  Serial.println("[HISTORY] Initialisiere SPIFFS...");

  // Bei einem defekten/nicht mountbaren SPIFFS wird automatisch formatiert.
  // Dieses Verhalten ist für das Projekt ausdrücklich gewünscht.
  if(!SPIFFS.begin(false)){
    Serial.println("[HISTORY] SPIFFS Mount fehlgeschlagen - Dateisystem wird formatiert...");
    SPIFFS.end();

    if(!SPIFFS.begin(true)){
      Serial.println("[HISTORY] SPIFFS Formatierung/Mount FEHLER");
      return false;
    }

    Serial.println("[HISTORY] SPIFFS Formatierung: OK");
  }

  historySpiffsReady=true;
  Serial.printf("[HISTORY] SPIFFS OK: total=%u used=%u frei=%u Byte\n",
                (unsigned)SPIFFS.totalBytes(),
                (unsigned)SPIFFS.usedBytes(),
                (unsigned)(SPIFFS.totalBytes()-SPIFFS.usedBytes()));
  return true;
}
static uint32_t historyDayKeyNow(){
  time_t now=time(nullptr); if(now<1000000000UL)return 0;
  struct tm t; localtime_r(&now,&t);
  return (uint32_t)(t.tm_year+1900)*10000UL+(uint32_t)(t.tm_mon+1)*100UL+(uint32_t)t.tm_mday;
}

// Wandelt YYYYMMDD + Tagesoffset über die Systemzeit um.
// Die CSV-Importprüfung selbst verwendet bewusst nur Tag 1-31 und Monat 1-12.
static uint32_t historyDayKeyAddDays(uint32_t dayKey, int32_t days){
  if(dayKey == 0) return 0;
  struct tm t = {};
  t.tm_year = (int)(dayKey / 10000UL) - 1900;
  t.tm_mon  = (int)((dayKey / 100UL) % 100UL) - 1;
  t.tm_mday = (int)(dayKey % 100UL);
  t.tm_hour = 12;
  time_t ts = mktime(&t);
  if(ts == (time_t)-1) return 0;
  ts += (time_t)days * 86400;
  localtime_r(&ts, &t);
  return (uint32_t)(t.tm_year + 1900) * 10000UL +
         (uint32_t)(t.tm_mon + 1) * 100UL +
         (uint32_t)t.tm_mday;
}

static int32_t historyDayOffset(uint32_t baseDay, uint32_t dayKey){
  if(baseDay == 0 || dayKey == 0) return 0;
  struct tm a = {}, b = {};
  a.tm_year = (int)(baseDay / 10000UL) - 1900;
  a.tm_mon  = (int)((baseDay / 100UL) % 100UL) - 1;
  a.tm_mday = (int)(baseDay % 100UL);
  a.tm_hour = 12;
  b.tm_year = (int)(dayKey / 10000UL) - 1900;
  b.tm_mon  = (int)((dayKey / 100UL) % 100UL) - 1;
  b.tm_mday = (int)(dayKey % 100UL);
  b.tm_hour = 12;
  time_t ta = mktime(&a), tb = mktime(&b);
  if(ta == (time_t)-1 || tb == (time_t)-1) return 0;
  int64_t diff = (int64_t)difftime(tb, ta);
  return (int32_t)(diff / 86400);
}
static bool historyValidateLoaded(){
  if(historyCount>HISTORY_RAM_CACHE_DAYS||historyIndex>=HISTORY_RAM_CACHE_DAYS)return false;
  if(historyCount==0)return true;
  uint16_t valid=0; for(uint16_t i=0;i<HISTORY_RAM_CACHE_DAYS;i++){const auto&r=historyStore[i];if(r.dayKey&&isfinite(r.endLiters)&&isfinite(r.consumedLiters)&&isfinite(r.refillLiters))valid++;}
  return valid>=historyCount;
}
// Ein aktuelles History-File kann bei Importdaten echte Kalendertage ohne
// Messdatensatz enthalten. In der dichten SPIFFS-Darstellung sind solche
// Zwischen-Slots komplett 0. Nur Slot 0 darf dayOffset==0 als echten Basistag
// verwenden.
static bool historyCompactSlotPresent(const CompactHistoryDay &row, uint32_t physicalIndex) {
  if (physicalIndex == 0) return true;
  return row.dayOffset != 0;
}

// Vereinheitlicht alte/sequentiell importierte und dichte Kalenderlayouts.
// Danach liegt jeder Datensatz an Index == dayOffset; fehlende Tage bleiben 0.
// Rückwärtskonvertierung vermeidet einen zweiten 3650-Tage-Arbeitspuffer.
static void historyNormalizeCompactLayout(CompactHistoryDay *rows, uint16_t count) {
  if (!rows || count == 0) return;
  const uint16_t n = min(count, (uint16_t)HISTORY_DAYS);
  for (int i = (int)n - 1; i >= 0; --i) {
    CompactHistoryDay row = rows[i];
    const bool present = historyCompactSlotPresent(row, (uint32_t)i);
    rows[i] = CompactHistoryDay{};
    if (!present) continue;
    if (row.dayOffset >= HISTORY_DAYS) continue;
    rows[row.dayOffset] = row;
  }
}

// Liest genau einen physischen History-Slot und konvertiert auch ältere
// persistente Formate in das aktuelle CompactHistoryDay-Schema.
static bool historyReadAnyCompactRecord(File &f, uint32_t version,
                                        uint32_t physicalIndex,
                                        CompactHistoryDay &row,
                                        bool &present) {
  row = CompactHistoryDay{};
  present = false;

  if (version == HISTORY_SPIFFS_VERSION) {
    if (f.read((uint8_t*)&row, sizeof(row)) != sizeof(row)) return false;
    present = historyCompactSlotPresent(row, physicalIndex);
    return true;
  }

  if (version == 0x00060202UL) {
    V625CompactHistoryDay old{};
    if (f.read((uint8_t*)&old, sizeof(old)) != sizeof(old)) return false;
    row.dayOffset = old.dayOffset;
    row.levelLiters = old.levelLiters;
    row.consumptionLiters = (uint16_t)lroundf(old.consumption100 / 100.0f);
    row.refillLiters = old.refillLiters;
    row.source = old.source <= HISTORY_TEST ? old.source : HISTORY_MEASURED;
    present = (physicalIndex == 0) || old.dayOffset != 0;
    return true;
  }

  if (version == 0x00060201UL) {
    PriorCompactHistoryDay old{};
    if (f.read((uint8_t*)&old, sizeof(old)) != sizeof(old)) return false;
    row.dayOffset = old.dayOffset;
    row.levelLiters = old.levelLiters;
    row.consumptionLiters = (uint16_t)lroundf(old.consumption100 / 100.0f);
    row.refillLiters = (uint16_t)lroundf(old.refill100 / 100.0f);
    row.source = old.source <= HISTORY_TEST ? old.source : HISTORY_MEASURED;
    present = (physicalIndex == 0) || old.dayOffset != 0;
    return true;
  }

  if (version == 0x00060002UL || version == 0x00060100UL) {
    LegacyCompactHistoryDay old{};
    if (f.read((uint8_t*)&old, sizeof(old)) != sizeof(old)) return false;
    row.dayOffset = old.dayOffset;
    row.levelLiters = (uint16_t)lroundf(old.level10 / 10.0f);
    row.consumptionLiters = (uint16_t)lroundf(old.consumption100 / 100.0f);
    row.refillLiters = (uint16_t)lroundf(old.refill100 / 100.0f);
    row.source = HISTORY_MEASURED;
    present = (physicalIndex == 0) || old.dayOffset != 0;
    return true;
  }

  return false;
}

static bool historyLoadFullCompact(const char *path, HistoryFileHeader &h, CompactHistoryDay *out) {
  File f = SPIFFS.open(path, FILE_READ);
  if (!f) return false;
  if (f.read((uint8_t*)&h, sizeof(h)) != sizeof(h)) { f.close(); return false; }
  const bool current = h.magic == HISTORY_SPIFFS_MAGIC && h.version == HISTORY_SPIFFS_VERSION &&
                       h.dataBytes == sizeof(CompactHistoryDay) * HISTORY_DAYS;
  const bool v625 = h.magic == HISTORY_SPIFFS_MAGIC && h.version == 0x00060202UL &&
                    h.dataBytes == sizeof(V625CompactHistoryDay) * HISTORY_DAYS;
  const bool prior = h.magic == HISTORY_SPIFFS_MAGIC && h.version == 0x00060201UL &&
                     h.dataBytes == sizeof(PriorCompactHistoryDay) * HISTORY_DAYS;
  const bool legacy = h.magic == HISTORY_SPIFFS_MAGIC &&
                      (h.version == 0x00060002UL || h.version == 0x00060100UL) &&
                      (h.dataBytes == sizeof(LegacyCompactHistoryDay) * HISTORY_DAYS ||
                       h.dataBytes == sizeof(LegacyCompactHistoryDay) * 365UL);
  if(!current && !v625 && !prior && !legacy){f.close();return false;}
  memset(out,0,sizeof(CompactHistoryDay)*HISTORY_DAYS);
  if(current){
    bool ok=f.read((uint8_t*)out,sizeof(CompactHistoryDay)*HISTORY_DAYS)==sizeof(CompactHistoryDay)*HISTORY_DAYS;
    f.close();
    if (ok) historyNormalizeCompactLayout(out, (uint16_t)min((uint32_t)h.count, (uint32_t)HISTORY_DAYS));
    return ok;
  }
  const uint32_t count=min((uint32_t)h.count,(uint32_t)HISTORY_DAYS);
  for(uint32_t i=0;i<count;i++){
    if(v625){
      V625CompactHistoryDay old{};
      if(f.read((uint8_t*)&old,sizeof(old))!=sizeof(old)){f.close();return false;}
      out[i].dayOffset=old.dayOffset; out[i].levelLiters=old.levelLiters;
      out[i].consumptionLiters=(uint16_t)lroundf(old.consumption100/100.0f);
      out[i].refillLiters=old.refillLiters;
      out[i].source=old.source<=HISTORY_TEST?old.source:HISTORY_MEASURED;
    } else if(prior){
      PriorCompactHistoryDay old{};
      if(f.read((uint8_t*)&old,sizeof(old))!=sizeof(old)){f.close();return false;}
      out[i].dayOffset=old.dayOffset; out[i].levelLiters=old.levelLiters;
      out[i].consumptionLiters=(uint16_t)lroundf(old.consumption100/100.0f);
      out[i].refillLiters=(uint16_t)lroundf(old.refill100/100.0f);
      out[i].source=old.source<=HISTORY_TEST?old.source:HISTORY_MEASURED;
    } else {
      LegacyCompactHistoryDay old{};
      if(f.read((uint8_t*)&old,sizeof(old))!=sizeof(old)){f.close();return false;}
      out[i].dayOffset=old.dayOffset; out[i].levelLiters=(uint16_t)lroundf(old.level10/10.0f);
      out[i].consumptionLiters=(uint16_t)lroundf(old.consumption100/100.0f);
      out[i].refillLiters=(uint16_t)lroundf(old.refill100/100.0f);
      out[i].source=HISTORY_MEASURED;
    }
  }
  f.close();
  historyNormalizeCompactLayout(out, (uint16_t)count);
  return true;
}

static void historyCompactFromEntry(CompactHistoryDay &c, const HistoryEntry &r, uint32_t baseDay) {
  int32_t d = historyDayOffset(baseDay, r.dayKey);
  c.dayOffset = (uint16_t)constrain(d, 0, 65535);
  c.levelLiters = (uint16_t)constrain((int)lroundf(max(0.0f, r.endLiters)), 0, 65535);
  c.consumptionLiters = (uint16_t)constrain((int)lroundf(max(0.0f, r.consumedLiters)), 0, 65535);
  c.refillLiters = (uint16_t)constrain((int)lroundf(max(0.0f, r.refillLiters)), 0, 65535);
  c.source = r.source <= HISTORY_TEST ? r.source : HISTORY_MEASURED;
}

static bool historyReadFile(const char*path, void *outPtr){
  HistoryFileHeader &out = *reinterpret_cast<HistoryFileHeader *>(outPtr);
  if(!historyEnsureSpiffs()) return false;

  HistoryFileHeader h{};
  CompactHistoryDay *full = historyWorkspace.compact;

  // Zentraler Loader konvertiert ältere Formate und normalisiert sowohl
  // sequentiell importierte als auch dichte Kalenderlayouts.
  if(!historyLoadFullCompact(path, h, full)) return false;
  if(h.count > HISTORY_DAYS) return false;

  memset(historyStore, 0, sizeof(historyStore));

  // Nur tatsächlich vorhandene Tage zählen. Leere Kalenderslots dürfen den
  // 365-Tage-RAM-Cache nicht auffüllen, sonst würden echte Datensätze
  // verdrängt und historyValidateLoaded() könnte fehlschlagen.
  uint32_t validCount = 0;
  for(uint32_t i=0; i<HISTORY_DAYS; ++i) {
    const CompactHistoryDay &row = full[i];
    const bool present = (i == 0)
      ? (h.count > 0 && row.dayOffset == 0)
      : (row.dayOffset == i);
    if(present) ++validCount;
  }

  const uint32_t skipValid = validCount > HISTORY_RAM_CACHE_DAYS
    ? validCount - HISTORY_RAM_CACHE_DAYS
    : 0;

  uint32_t seen = 0;
  uint16_t dst = 0;
  for(uint32_t i=0; i<HISTORY_DAYS && dst<HISTORY_RAM_CACHE_DAYS; ++i) {
    const CompactHistoryDay &row = full[i];
    const bool present = (i == 0)
      ? (h.count > 0 && row.dayOffset == 0)
      : (row.dayOffset == i);
    if(!present) continue;

    if(seen++ < skipValid) continue;

    HistoryEntry &r = historyStore[dst++];
    r.dayKey = historyDayKeyAddDays(h.baseDay, row.dayOffset);
    r.endLiters = row.levelLiters;
    r.consumedLiters = row.consumptionLiters;
    r.refillLiters = row.refillLiters;
    r.source = row.source <= HISTORY_TEST ? row.source : HISTORY_MEASURED;
  }

  historyCount = dst;
  historyIndex = (historyCount == HISTORY_RAM_CACHE_DAYS) ? 0 : historyCount;
  historyLastDayKey = h.lastDayKey;
  historyDayStartLiters = h.dayStartLiters;
  historyLastLiters = h.lastLiters;
  historyPendingRefill = h.pendingRefill;
  out = h;

  return historyValidateLoaded();
}

static bool historyGetActiveFile(const char **pathOut, HistoryFileHeader *headerOut = nullptr);

static bool historyWriteFile(const char*tmp,const char*final,uint32_t gen){
  if(!historyEnsureSpiffs())return false;

  CompactHistoryDay *full = historyWorkspace.compact;
  memset(full,0,sizeof(CompactHistoryDay)*HISTORY_DAYS);
  uint32_t base=0;
  HistoryFileHeader old{};
  const char *oldPath=nullptr;
  if(historyGetActiveFile(&oldPath,&old)) {
    if(!historyLoadFullCompact(oldPath,old,full)) memset(full,0,sizeof(CompactHistoryDay)*HISTORY_DAYS);
    else base=old.baseDay;
  }

  if(base==0 && historyCount) {
    base=historyStore[(historyIndex+HISTORY_RAM_CACHE_DAYS-historyCount)%HISTORY_RAM_CACHE_DAYS].dayKey;
  }
  // Auch am allerersten Messtag muss der laufende Tageszustand
  // persistiert werden koennen, obwohl noch kein abgeschlossener Tag existiert.
  if(base==0 && historyLastDayKey) base=historyLastDayKey;
  if(base==0) return false;

  uint32_t lastDay=historyLastDayKey;
  uint32_t maxDay=historyDayKeyAddDays(base,HISTORY_DAYS-1);
  if(lastDay>maxDay) {
    int32_t shift=historyDayOffset(base,lastDay)-(HISTORY_DAYS-1);
    if(shift>0 && shift<HISTORY_DAYS) {
      memmove(full,full+shift,sizeof(CompactHistoryDay)*(HISTORY_DAYS-shift));
      memset(full+(HISTORY_DAYS-shift),0,sizeof(CompactHistoryDay)*shift);
      base=historyDayKeyAddDays(base,shift);
    }
  }

  for(uint16_t i=0;i<historyCount && i<HISTORY_RAM_CACHE_DAYS;i++){
    uint16_t si=(historyIndex+HISTORY_RAM_CACHE_DAYS-historyCount+i)%HISTORY_RAM_CACHE_DAYS;
    const HistoryEntry &r=historyStore[si];
    if(!r.dayKey) continue;
    int32_t off=historyDayOffset(base,r.dayKey);
    if(off<0 || off>=HISTORY_DAYS) continue;
    historyCompactFromEntry(full[off],r,base);
  }

  uint16_t count=0;
  for(uint16_t i=0;i<HISTORY_DAYS;i++) if(full[i].levelLiters||full[i].dayOffset||i==0) {
    if(full[i].source<=HISTORY_TEST && (full[i].levelLiters||full[i].dayOffset||full[i].consumptionLiters||full[i].refillLiters)) count=i+1;
  }
  if(count==0 && historyCount>0) count=historyCount;

  HistoryFileHeader h{};
  h.magic=HISTORY_SPIFFS_MAGIC;h.version=HISTORY_SPIFFS_VERSION;h.generation=gen;
  h.count=count;h.index=historyIndex;h.lastDayKey=historyLastDayKey;
  h.dayStartLiters=historyDayStartLiters;h.lastLiters=historyLastLiters;h.pendingRefill=historyPendingRefill;
  h.baseDay=base;h.dataBytes=sizeof(CompactHistoryDay)*HISTORY_DAYS;h.dataCrc=historyCrc32((uint8_t*)full,h.dataBytes);

  // Die inaktive Zielbank VOR dem Schreiben der temporaeren Datei
  // entfernen. Dadurch existieren waehrend des Commits nur die aktive Bank
  // und die neue TMP-Datei. Das vermeidet Platzmangel im ~128-kB-SPIFFS.
  SPIFFS.remove(tmp);
  SPIFFS.remove(final);
  File f=SPIFFS.open(tmp,FILE_WRITE);if(!f)return false;
  bool ok=f.write((uint8_t*)&h,sizeof(h))==sizeof(h)&&f.write((uint8_t*)full,sizeof(CompactHistoryDay)*HISTORY_DAYS)==sizeof(CompactHistoryDay)*HISTORY_DAYS;
  f.flush();f.close();if(!ok){SPIFFS.remove(tmp);return false;}
  File v=SPIFFS.open(tmp,FILE_READ);if(!v){SPIFFS.remove(tmp);return false;}
  HistoryFileHeader vh{};bool verified=v.read((uint8_t*)&vh,sizeof(vh))==sizeof(vh);
  if(verified)verified=vh.magic==HISTORY_SPIFFS_MAGIC&&vh.version==HISTORY_SPIFFS_VERSION&&vh.count==h.count&&vh.dataBytes==h.dataBytes&&vh.dataCrc==h.dataCrc;
  v.close();if(!verified){SPIFFS.remove(tmp);return false;}
  SPIFFS.remove(final);if(!SPIFFS.rename(tmp,final)){SPIFFS.remove(tmp);return false;}return true;
}

static bool saveHistoryStore(){
  if(!historyStoreDirty)return true;
  if(!historyEnsureSpiffs())return false;

  Preferences m;
  uint8_t active=0;
  uint32_t gen=0;
  if(m.begin(HISTORY_NVS_META_NS,false)){
    active=m.getUChar("active",0);
    gen=m.getUInt("gen",0);
    m.end();
  }

  const uint8_t oldActive=active;
  const uint32_t oldGen=gen;
  const uint8_t target=active?0:1;
  const uint32_t newGen=gen+1;
  const char*tmp=target?HISTORY_SPIFFS_TMP_B:HISTORY_SPIFFS_TMP_A;
  const char*final=target?HISTORY_SPIFFS_FILE_B:HISTORY_SPIFFS_FILE_A;

  Serial.printf("[HISTORY] Speichere %u Tage in SPIFFS-Bank %c...\n",historyCount,target?'B':'A');

  if(!historyWriteFile(tmp,final,newGen)){
    Serial.println("[HISTORY] Persistenz-Verifikation: FEHLER");
    SPIFFS.remove(tmp);
    SPIFFS.remove(final);
    return false;
  }

  if(!m.begin(HISTORY_NVS_META_NS,false)){
    SPIFFS.remove(final);
    return false;
  }

  bool ok = (m.putUChar("active",target)==1) &&
            (m.putUInt("gen",newGen)==sizeof(uint32_t));

  if(!ok){
    // Metadaten nicht vollständig geschrieben: alten Zustand wiederherstellen.
    m.putUChar("active",oldActive);
    m.putUInt("gen",oldGen);
    m.end();
    SPIFFS.remove(final);
    Serial.println("[HISTORY] Persistenz-Metadaten FEHLER - alter Zustand bleibt aktiv");
    return false;
  }

  m.end();
  historyStoreDirty=false;
  Serial.printf("[HISTORY] Persistenz-Verifikation: OK, Bank=%c, Gen=%lu\n",target?'B':'A',(unsigned long)newGen);
  return true;
}
static void loadHistoryStore(){
  memset(historyStore,0,sizeof(historyStore));historyIndex=0;historyCount=0;historyLastDayKey=0;historyDayStartLiters=NAN;historyLastLiters=NAN;historyPendingRefill=0;historyStoreDirty=false;
  if(!historyEnsureSpiffs()){Serial.println("[HISTORY] SPIFFS nicht verfügbar - Historie konnte nicht geladen werden");return;}
  Preferences m;uint8_t active=255;uint32_t gen=0;if(m.begin(HISTORY_NVS_META_NS,true)){active=m.getUChar("active",255);gen=m.getUInt("gen",0);m.end();}
  const char*paths[2]={HISTORY_SPIFFS_FILE_A,HISTORY_SPIFFS_FILE_B};uint8_t order[2]={0,1};if(active<=1){order[0]=active;order[1]=active?0:1;}
  bool ok=false;HistoryFileHeader best{};uint8_t bestBank=0;
  for(uint8_t n=0;n<2;n++){HistoryFileHeader h{};if(historyReadFile(paths[order[n]],&h)&&(!ok||h.generation>best.generation)){best=h;bestBank=order[n];ok=true;}}
  if(ok){
    bool legacyLoaded = (best.version == 0x00060002UL || best.version == 0x00060100UL);

    Serial.printf(
      "[HISTORY] SPIFFS-Historie geladen: %u Tage, Gen=%lu\n",
      historyCount,
      (unsigned long)best.generation
    );

    if(m.begin(HISTORY_NVS_META_NS,false)){
      m.putUChar("active",bestBank);
      m.putUInt("gen",best.generation);
      m.end();
    }

    if(legacyLoaded){
      // Alte 365-Tage-Historie wird beim nächsten Speichervorgang
      // automatisch in das aktuelle 3650-Tage-Format überführt.
      historyStoreDirty = true;

      Serial.println("[HISTORY] Speichere migrierte Historie im aktuellen 3650-Tage-Format...");
      if(saveHistoryStore()){
        Serial.println("[HISTORY] Migration Altformat -> aktuelles Format: OK");
      } else {
        Serial.println("[HISTORY] Migration Altformat -> aktuelles Format: FEHLER");
      }
    }
  }
  else Serial.println("[HISTORY] Keine gültige persistente SPIFFS-Historie vorhanden");
}

static void startHistoryDay(float liters) {
  uint32_t key = historyDayKeyNow();
  if (key == 0 || !isfinite(liters) || liters < 0.0f) return;

  historyLastDayKey = key;
  historyDayStartLiters = liters;
  historyLastLiters = liters;
  historyPendingRefill = 0.0f;

  // Der Startwert des laufenden Tages ist Teil der Persistenz.
  historyStoreDirty = true;
}

static void closeHistoryDay(float endLiters) {
  if (historyLastDayKey == 0 ||
      !isfinite(historyDayStartLiters) ||
      !isfinite(endLiters)) {
    return;
  }

  HistoryEntry &entry = historyStore[historyIndex];
  entry.dayKey = historyLastDayKey;
  entry.endLiters = endLiters;

  // Verbrauch berücksichtigt Nachfüllungen: Start + Nachfüllung - Ende.
  float consumed = historyDayStartLiters + max(0.0f, historyPendingRefill) - endLiters;
  if (!isfinite(consumed) || consumed < 0.0f) consumed = 0.0f;

  entry.consumedLiters = consumed;
  entry.refillLiters = max(0.0f, historyPendingRefill);
  entry.source = HISTORY_MEASURED;

  historyIndex++;
  if (historyIndex >= HISTORY_RAM_CACHE_DAYS) historyIndex = 0;

  if (historyCount < HISTORY_RAM_CACHE_DAYS) historyCount++;

  historyStoreDirty = true;
  saveHistoryStore();

  historyDayStartLiters = endLiters;
  historyLastLiters = endLiters;
  historyPendingRefill = 0.0f;
}

void updateDailyHistory(float liters) {
  if (!isfinite(liters) || liters < 0.0f) return;

  uint32_t key = historyDayKeyNow();
  if (key == 0) return;

  if (!isfinite(historyDayStartLiters)) {
    startHistoryDay(liters);
    // Der erste Tages-Startwert sofort sichern. Das ist nur einmal pro neuem
    // Tageszustand nötig und verhindert Verbrauchsverlust nach frühem Reset.
    if (saveHistoryStore()) historyLastLiveSaveMs = millis();
    return;
  }

  if (key != historyLastDayKey) {
    closeHistoryDay(historyLastLiters);
    startHistoryDay(liters);
    // Nach dem Tageswechsel den neuen Startwert sofort sichern.
    if (saveHistoryStore()) historyLastLiveSaveMs = millis();
    return;
  }

  // Nachfüllungen werden ausschließlich von updateRefillDetection()
  // erkannt und über confirmedRefillPendingLiters hierher übergeben.
  // Eine zweite Delta-Erkennung würde Nachfüllungen doppelt zählen.
  historyLastLiters = liters;
  historyStoreDirty = true;

  // Auch innerhalb des aktuellen Tages regelmäßig persistent speichern.
  // Dadurch bleibt der Tagesverbrauch nach Reset/Stromausfall möglichst aktuell.
  const uint32_t nowMs = millis();
  if (historyStoreDirty &&
      (historyLastLiters == 0 ||
       (uint32_t)(nowMs - historyLastLiveSaveMs) >= HISTORY_LIVE_SAVE_INTERVAL)) {
    if (saveHistoryStore()) {
      historyLastLiveSaveMs = nowMs;
    }
  }
}

static float historyTodayConsumption() {
  if (!isfinite(historyDayStartLiters) || !isfinite(historyLastLiters)) return 0.0f;
  return max(0.0f, historyDayStartLiters + max(0.0f, historyPendingRefill) - historyLastLiters);
}

static float historyAverageDays(uint16_t days) {
  if (days == 0) return 0.0f;
  if (days > HISTORY_RAM_CACHE_DAYS) days = HISTORY_RAM_CACHE_DAYS;

  float sum = 0.0f;
  uint16_t count = 0;
  uint16_t historicalDays = days;

  // Heute gehoert in das Fenster und ersetzt einen historischen Tag.
  if (isfinite(historyDayStartLiters) && isfinite(historyLastLiters)) {
    sum += historyTodayConsumption();
    count++;
    historicalDays = days > 0 ? days - 1 : 0;
  }

  if (historicalDays > historyCount) historicalDays = historyCount;

  int idx = (int)historyIndex - 1;
  if (idx < 0) idx = HISTORY_RAM_CACHE_DAYS - 1;

  for (uint16_t n = 0; n < historicalDays; n++) {
    const HistoryEntry &r = historyStore[idx];
    if (r.dayKey != 0) {
      sum += max(0.0f, r.consumedLiters);
      count++;
    }
    if (--idx < 0) idx = HISTORY_RAM_CACHE_DAYS - 1;
  }

  return count ? sum / count : 0.0f;
}

static float historyAverage7Days()   { return historyAverageDays(7); }
static float historyAverage30Days()  { return historyAverageDays(30); }
static float historyAverage90Days()  { return historyAverageDays(90); }
static float historyAverage365Days() { return historyAverageDays(365); }

static float historyRefillDays(uint16_t days) {
  if (days == 0) return 0.0f;
  if (days > historyCount) days = historyCount;

  float sum = historyPendingRefill;
  int idx = (int)historyIndex - 1;
  if (idx < 0) idx = HISTORY_RAM_CACHE_DAYS - 1;

  for (uint16_t n = 0; n < days; n++) {
    const HistoryEntry &r = historyStore[idx];
    if (r.dayKey != 0) sum += max(0.0f, r.refillLiters);
    if (--idx < 0) idx = HISTORY_RAM_CACHE_DAYS - 1;
  }
  return sum;
}

static float historyDaysRemaining(float liters) {
  if (!isfinite(liters) || liters <= 0.0f) return -1.0f;

  float avg = historyAverage7Days();
  if (avg <= 0.01f) avg = historyAverage30Days();
  if (avg <= 0.01f) avg = historyAverage90Days();
  if (avg <= 0.01f) avg = historyAverage365Days();

  return avg > 0.01f ? liters / avg : -1.0f;
}

// ============================================================================
// VERBRAUCHS-API – EINZIGE DATENQUELLE: 10-JAHRES-HISTORIE
// ============================================================================

float consumptionToday() {
  return historyTodayConsumption();
}

static float historySumLastDays(uint16_t days) {
  if (days == 0) return 0.0f;
  if (days > HISTORY_RAM_CACHE_DAYS) days = HISTORY_RAM_CACHE_DAYS;

  // Ein Zeitraum von 7 Tagen bedeutet heute + 6 abgeschlossene Tage.
  float sum = 0.0f;
  uint16_t historicalDays = days;
  if (isfinite(historyDayStartLiters) && isfinite(historyLastLiters)) {
    sum += historyTodayConsumption();
    historicalDays = days > 0 ? days - 1 : 0;
  }

  uint16_t available = historyCount;
  if (available > historicalDays) available = historicalDays;

  for (uint16_t i = 0; i < available; ++i) {
    int idx = (int)historyIndex - 1 - (int)i;
    while (idx < 0) idx += HISTORY_RAM_CACHE_DAYS;

    const float v = historyStore[idx].consumedLiters;
    if (isfinite(v) && v >= 0.0f) sum += v;
  }
  return sum;
}

float consumptionWeek() {
  return historySumLastDays(7);
}

float consumptionMonth() {
  return historySumLastDays(30);
}

// Nettoverbrauch fuer persistente laengere Zeitraeume.
// Tages-Endwerte werden nicht einzeln aufsummiert, sondern als Bilanz:
// Startbestand + Nachfuellungen - Endbestand.
// Dadurch mittelt sich Messrauschen weitgehend heraus.
static float historyNetConsumptionLastDays(uint16_t days) {
  if (days == 0) return 0.0f;
  if (days > HISTORY_RAM_CACHE_DAYS) days = HISTORY_RAM_CACHE_DAYS;

  const bool haveLive = isfinite(historyDayStartLiters) && isfinite(historyLastLiters);
  uint16_t completedWanted = haveLive && days > 0 ? days - 1 : days;
  if (completedWanted > historyCount) completedWanted = historyCount;

  float startLiters = NAN;
  float endLiters = NAN;
  float refillSum = 0.0f;

  if (completedWanted > 0) {
    int oldestIdx = (int)historyIndex - (int)completedWanted;
    while (oldestIdx < 0) oldestIdx += HISTORY_RAM_CACHE_DAYS;

    const HistoryEntry &oldest = historyStore[oldestIdx];

    // Tagesstart des aeltesten einbezogenen Tages aus dessen eigener Bilanz:
    // Verbrauch = Start + Nachfuellung - Ende
    if (isfinite(oldest.endLiters)) {
      const float cons = isfinite(oldest.consumedLiters) ? max(0.0f, oldest.consumedLiters) : 0.0f;
      const float refill = isfinite(oldest.refillLiters) ? max(0.0f, oldest.refillLiters) : 0.0f;
      startLiters = oldest.endLiters + cons - refill;
    }

    for (uint16_t n = 0; n < completedWanted; ++n) {
      int idx = oldestIdx + n;
      while (idx >= HISTORY_RAM_CACHE_DAYS) idx -= HISTORY_RAM_CACHE_DAYS;
      const HistoryEntry &r = historyStore[idx];
      if (isfinite(r.refillLiters)) refillSum += max(0.0f, r.refillLiters);
      if (isfinite(r.endLiters)) endLiters = r.endLiters;
    }
  }

  if (haveLive) {
    if (!isfinite(startLiters)) startLiters = historyDayStartLiters;
    refillSum += max(0.0f, historyPendingRefill);
    endLiters = historyLastLiters;
  }

  if (!isfinite(startLiters) || !isfinite(endLiters)) return 0.0f;
  return max(0.0f, startLiters + refillSum - endLiters);
}

static float historySumConsumptionFromSpiffs(uint16_t days) {
  if (!historySpiffsReady || days == 0) return 0.0f;
  if (days > HISTORY_DAYS) days = HISTORY_DAYS;

  const char *path = nullptr;
  HistoryFileHeader h{};
  if (!historyGetActiveFile(&path, &h) || !path || !path[0]) return 0.0f;

  File f = SPIFFS.open(path, FILE_READ);
  if (!f) return 0.0f;

  HistoryFileHeader fileHeader{};
  if (f.read((uint8_t*)&fileHeader, sizeof(fileHeader)) != sizeof(fileHeader)) {
    f.close();
    return 0.0f;
  }

  const bool current = fileHeader.version == HISTORY_SPIFFS_VERSION;
  const bool v625 = fileHeader.version == 0x00060202UL;
  const bool prior = fileHeader.version == 0x00060201UL;
  const bool legacy = fileHeader.version == 0x00060002UL ||
                      fileHeader.version == 0x00060100UL;

  if (!current && !v625 && !prior && !legacy) {
    f.close();
    return 0.0f;
  }

  const uint32_t available = min((uint32_t)fileHeader.count,
                                 (uint32_t)HISTORY_DAYS);
  uint32_t wanted = days;

  // Wenn der aktuelle Tag bereits live mitgerechnet werden kann, nur
  // days-1 abgeschlossene Tage aus SPIFFS lesen.
  const bool haveLive = isfinite(historyDayStartLiters) &&
                        isfinite(historyLastLiters);
  if (haveLive && wanted > 0) wanted--;

  if (wanted > available) wanted = available;
  const uint32_t first = available - wanted;

  const size_t recSize =
      legacy ? sizeof(LegacyCompactHistoryDay) :
      (prior ? sizeof(PriorCompactHistoryDay) :
      (v625 ? sizeof(V625CompactHistoryDay) :
              sizeof(CompactHistoryDay)));

  if (!f.seek(sizeof(HistoryFileHeader) + first * recSize, SeekSet)) {
    f.close();
    return 0.0f;
  }

  float sum = 0.0f;

  for (uint32_t i = 0; i < wanted; ++i) {
    float cons = 0.0f;

    if (legacy) {
      LegacyCompactHistoryDay row{};
      if (f.read((uint8_t*)&row, sizeof(row)) != sizeof(row)) break;
      cons = row.consumption100 / 100.0f;
    } else if (prior) {
      PriorCompactHistoryDay row{};
      if (f.read((uint8_t*)&row, sizeof(row)) != sizeof(row)) break;
      cons = row.consumption100 / 100.0f;
    } else if (v625) {
      V625CompactHistoryDay row{};
      if (f.read((uint8_t*)&row, sizeof(row)) != sizeof(row)) break;
      cons = row.consumption100 / 100.0f;
    } else {
      CompactHistoryDay row{};
      if (f.read((uint8_t*)&row, sizeof(row)) != sizeof(row)) break;
      cons = row.consumptionLiters;
    }

    if (isfinite(cons) && cons >= 0.0f) sum += cons;
  }

  f.close();

  if (haveLive) {
    const float live = historyTodayConsumption();
    if (isfinite(live) && live >= 0.0f) sum += live;
  }

  return sum;
}

float consumption365Days() {
  return historySumConsumptionFromSpiffs(365);
}

float consumptionAverage7Days() {
  return historyAverage7Days();
}

float consumptionAverage30Days() {
  return historyAverage30Days();
}


// Rekonstruiert die separate Nachfüll-Ereignisliste ausschließlich aus den
// tatsächlich persistent gespeicherten History-Tageswerten. Dadurch können
// alte NVS-Ereignisse nicht mehr als Phantom-Nachfüllungen erscheinen.
static bool rebuildRefillEventsFromPersistentHistory() {
  const char *path = nullptr;
  HistoryFileHeader h{};

  if(!historyGetActiveFile(&path, &h) || !path || !path[0]) {
    return false;
  }

  File f = SPIFFS.open(path, FILE_READ);
  if(!f) return false;

  if(!f.seek(sizeof(HistoryFileHeader), SeekSet)) {
    f.close();
    return false;
  }

  RefillEvent found[REFILL_EVENT_COUNT] = {};
  uint8_t foundCount = 0;

  // Datei chronologisch lesen. Für die Ereignisliste werden die neuesten
  // REFILL_EVENT_COUNT Treffer behalten.
  const uint32_t physicalCount =
      min((uint32_t)h.count, (uint32_t)HISTORY_DAYS);

  for(uint32_t i=0; i<physicalCount; ++i) {
    CompactHistoryDay row{};
    bool present = false;

    if(!historyReadAnyCompactRecord(f, h.version, i, row, present)) break;
    if(!present) continue;

    const float refill = (float)row.refillLiters;
    if(!isfinite(refill) || refill < REFILL_MIN_LITERS) continue;

    // Eine Nachfüllmenge größer als die gesamte Tankkapazität ist unmöglich
    // und kennzeichnet korrupte/fehlinterpretierte Persistenzdaten.
    const float capacity = calculateTankCapacityLiters();
    if(isfinite(capacity) && capacity > 0.0f && refill > capacity) continue;

    const uint32_t dayKey = historyDayKeyAddDays(h.baseDay, row.dayOffset);
    if(!dayKey) continue;

    // Historische Ereignisse dürfen nicht in der Zukunft liegen.
    const uint32_t todayKey = historyDayKeyNow();
    if(todayKey && dayKey > todayKey) continue;

    // Ebenso niemals außerhalb des vom Header definierten History-Bereichs.
    if(h.lastDayKey && dayKey > h.lastDayKey) continue;

    const uint32_t ts = historyDayKeyToTimestamp(dayKey);
    if(!ts) continue;

    // Neueste Ereignisse vorne halten.
    const uint8_t limit =
        foundCount < REFILL_EVENT_COUNT ? foundCount : REFILL_EVENT_COUNT - 1;

    for(int j=(int)limit; j>0; --j) {
      found[j] = found[j-1];
    }

    found[0].timestamp = ts;
    found[0].liters = refill;

    if(foundCount < REFILL_EVENT_COUNT) ++foundCount;
  }

  f.close();

  clearRefillEvents();
  refillEventCount = foundCount;
  for(uint8_t i=0; i<foundCount; ++i) {
    refillEvents[i] = found[i];
  }

  if(refillEventCount > 0) {
    lastConfirmedRefillLiters = refillEvents[0].liters;
    lastRefillTimestamp = refillEvents[0].timestamp;
  }

  confirmedRefillPendingLiters = 0.0f;

  // NVS nur noch als Spiegel der aus History abgeleiteten Wahrheit speichern.
  saveRefillEvents();

  Serial.printf("[REFILL] Ereignisliste aus persistenter Historie aufgebaut: %u Ereignisse\n",
                (unsigned)refillEventCount);
  return true;
}


void initConsumptionTracking() {
  // Historische Nachfüllungen stammen primär aus der persistenten History.
  // Nur wenn noch keine History vorhanden/lesbar ist, dient die alte NVS-Liste
  // als Fallback (z.B. bei einer fabrikneuen Installation).
  if(!rebuildRefillEventsFromPersistentHistory()) {
    loadRefillEvents();
  }
}



static bool historyReadDayFromSpiffs(uint32_t dayKey, HistoryEntry &out) {
  if (!historySpiffsReady || dayKey == 0) return false;

  const char *path = nullptr;
  HistoryFileHeader activeHeader{};
  if (!historyGetActiveFile(&path, &activeHeader) || !path || !path[0]) return false;

  File f = SPIFFS.open(path, FILE_READ);
  if (!f) return false;

  HistoryFileHeader h{};
  if (f.read((uint8_t*)&h, sizeof(h)) != sizeof(h) ||
      h.magic != HISTORY_SPIFFS_MAGIC ||
      h.version != HISTORY_SPIFFS_VERSION) {
    f.close();
    return false;
  }

  for (uint16_t i = 0; i < h.count; ++i) {
    CompactHistoryDay row{};
    if (f.read((uint8_t*)&row, sizeof(row)) != sizeof(row)) break;
    const uint32_t dk = historyDayKeyAddDays(h.baseDay, row.dayOffset);
    if (dk != dayKey) continue;

    out.dayKey = dk;
    out.endLiters = (float)row.levelLiters;
    out.consumedLiters = (float)row.consumptionLiters;
    out.refillLiters = (float)row.refillLiters;
    out.source = row.source;
    f.close();
    return true;
  }

  f.close();
  return false;
}

static float historyNetConsumptionFromSpiffs(uint16_t days) {
  if (!historySpiffsReady || days == 0) return 0.0f;

  const char *path = nullptr;
  HistoryFileHeader h{};
  if (!historyGetActiveFile(&path, &h) || !path || !path[0]) return 0.0f;

  File f = SPIFFS.open(path, FILE_READ);
  if (!f) return 0.0f;
  if (!f.seek(sizeof(HistoryFileHeader), SeekSet)) {
    f.close();
    return 0.0f;
  }

  const uint32_t todayKey = historyDayKeyNow();
  const uint32_t refDay = todayKey ? todayKey : h.lastDayKey;
  const uint32_t firstDay = refDay ? historyDayKeyAddDays(refDay, -((int32_t)days - 1)) : 0;

  float total = 0.0f;
  bool storedToday = false;
  const uint32_t count = min((uint32_t)h.count, (uint32_t)HISTORY_DAYS);

  for (uint32_t i = 0; i < count; ++i) {
    CompactHistoryDay row{};
    bool present = false;
    if (!historyReadAnyCompactRecord(f, h.version, i, row, present)) break;
    if (!present) continue;

    const uint32_t dayKey = historyDayKeyAddDays(h.baseDay, row.dayOffset);
    if (!dayKey) continue;
    if (firstDay && dayKey < firstDay) continue;
    if (refDay && dayKey > refDay) continue;

    total += max(0.0f, (float)row.consumptionLiters);
    if (todayKey && dayKey == todayKey) storedToday = true;
  }
  f.close();

  // Der laufende Tag steht normalerweise noch nicht als abgeschlossener
  // SPIFFS-Tagesdatensatz im File.
  if (todayKey && !storedToday) total += max(0.0f, consumptionToday());

  return total;
}

// WEB HISTORY
// Aufbewahrungsdauer fest auf 10 Jahre (3650 Tagesdatensaetze).
// ============================================================================

static bool historyGetActiveFile(const char **pathOut, HistoryFileHeader *headerOut) {
  if (!historyEnsureSpiffs()) return false;
  Preferences m;
  uint8_t active = 255;
  if (m.begin(HISTORY_NVS_META_NS, true)) {
    active = m.getUChar("active", 255);
    m.end();
  }
  const char *paths[2] = { HISTORY_SPIFFS_FILE_A, HISTORY_SPIFFS_FILE_B };
  bool found = false;
  HistoryFileHeader best{};
  uint8_t bestBank = 0;
  for (uint8_t n = 0; n < 2; ++n) {
    uint8_t bank = (active <= 1) ? (active == n ? active : (active ? 0 : 1)) : n;
    const char *path = paths[bank];
    File f = SPIFFS.open(path, FILE_READ);
    if (!f) continue;
    HistoryFileHeader h{};
    bool ok = f.read((uint8_t*)&h, sizeof(h)) == sizeof(h);
    f.close();
    if (!ok) continue;
    const uint32_t expectedBytes = sizeof(CompactHistoryDay) * HISTORY_DAYS;
    const uint32_t v625Bytes = sizeof(V625CompactHistoryDay) * HISTORY_DAYS;
  const uint32_t priorBytes = sizeof(PriorCompactHistoryDay) * HISTORY_DAYS;
    const uint32_t legacyBytes = sizeof(LegacyCompactHistoryDay) * HISTORY_DAYS;
    const uint32_t legacy365Bytes = sizeof(LegacyCompactHistoryDay) * 365UL;
    const bool valid = h.magic == HISTORY_SPIFFS_MAGIC &&
                       ((h.version == HISTORY_SPIFFS_VERSION && h.dataBytes == expectedBytes) ||
                        (h.version == 0x00060202UL && h.dataBytes == v625Bytes) ||
                        (h.version == 0x00060201UL && h.dataBytes == priorBytes) ||
                        ((h.version == 0x00060002UL || h.version == 0x00060100UL) &&
                         (h.dataBytes == legacyBytes || h.dataBytes == legacy365Bytes))) &&
                       h.count <= HISTORY_DAYS;
    if (!valid) continue;
    if (!found || h.generation > best.generation) {
      found = true;
      best = h;
      bestBank = bank;
    }
  }
  if (!found) return false;
  if (headerOut) *headerOut = best;
  *pathOut = paths[bestBank];
  return true;
}

void handleHistoryPage(){
  String html=pageHeader("Historie");html.reserve(11000);
  html+=R"HTML(<div class='nav'><a href='/'>Dashboard</a><a class='active' href='/history'>Historie</a><a href='/settings'>Einstellungen</a><a href='/systemstatus'>System</a></div>
<style>.periods{display:flex;gap:6px;flex-wrap:wrap}.periodBtn{background:#292929;border:1px solid #4b4b4b;border-radius:999px;padding:7px 12px;color:#eee;text-decoration:none}.periodBtn.active{background:#1769aa}.chartWrap{position:relative;height:330px}.chart{width:100%;height:100%;display:block}.tip{position:absolute;display:none;pointer-events:none;background:#101418;border:1px solid #4d5965;border-radius:9px;padding:9px;box-shadow:0 4px 14px #000;font-size:12px}.grid2{display:grid;grid-template-columns:repeat(4,1fr);gap:9px}.legend{display:flex;gap:16px;flex-wrap:wrap;align-items:center;margin-top:10px;font-size:13px;color:#ccc}.legend span{display:inline-flex;align-items:center;gap:6px}.legend i{display:inline-block;width:18px;height:4px;border-radius:3px}@media(max-width:700px){.grid2{grid-template-columns:repeat(2,1fr)}.chartWrap{height:260px}}</style>
<div class='card'><div class='topbar'><h1>Historie</h1>
<div class='periods'><a class='periodBtn' data-days='183'>½ Jahr</a><a class='periodBtn active' data-days='365'>1 Jahr</a><a class='periodBtn' data-days='1825'>5 Jahre</a><a class='periodBtn' data-days='3650'>10 Jahre</a></div></div><div class='chartWrap'><canvas id='hc' class='chart'></canvas><div id='ht' class='tip'></div></div><div class='legend'><span><i style='background:#4da6ff'></i>Füllstand</span><span><i style='background:#ffb52e'></i>Verbrauch</span><span><i style='background:#42d65b'></i>Nachfüllung</span></div><p style='opacity:.8;margin-top:8px'>Datenquelle: ● Gemessen &nbsp; ○ Import &nbsp; △ Test</p></div>
<div class='card'><div class='chartHead'><h2 style='margin:0'>Monatsvergleich nach Jahren</h2><div><label class='muted'>Jahre anzeigen: </label><select id='yearCount'><option value='3'>3</option><option value='5' selected>5</option><option value='10'>10</option></select></div></div><p class='muted'>Monatlicher Verbrauch in Litern. Die neuesten Jahre werden nebeneinander dargestellt.</p><div class='chartWrap' style='height:360px'><canvas id='monthChart' class='chart'></canvas><div id='monthTip' class='tip'></div></div><div style='margin-top:10px'><span class='muted'>Legende:</span><div id='monthLegend' class='legend' style='margin-top:6px'></div></div><details style='margin-top:12px'><summary>Wertetabelle anzeigen</summary><div style='overflow:auto;margin-top:8px'><table id='monthCompare'><tr><td>Lade Historie …</td></tr></table></div></details></div>
<div class='card'><h2>Letzte 5 Nachfüllvorgänge</h2><div id='recentRefills'><p class='muted'>Nachfüllvorgänge werden geladen …</p></div></div>
<div class='card'><h2>Statistik</h2><div class='grid2'><div class='metric-card'><h3>Zeitraum</h3><div id='sd'>--</div></div><div class='metric-card'><h3>Verbrauch</h3><div id='sc'>-- L</div></div><div class='metric-card'><h3>Nachfüllungen</h3><div id='sr'>-- L</div></div><div class='metric-card'><h3>Füllstand</h3><div id='sl'>--</div></div></div></div>
<div class='card'><h2>Analyse & Statistik</h2><div class='grid2'><div class='metric-card'><h3>Aktueller Monat</h3><div id='amCons'>-- L</div></div><div class='metric-card'><h3>Aktuelles Jahr</h3><div id='ayCons'>-- L</div></div><div class='metric-card'><h3>Ø Verbrauch / Tag</h3><div id='avgDay'>-- L</div><small>über verfügbare Historie</small></div><div class='metric-card'><h3>Vorjahr bis heute</h3><div id='prevYearCons'>-- L</div></div><div class='metric-card'><h3>Jahresvergleich</h3><div id='yearDelta'>--</div><small>gleicher Zeitraum wie aktuelles Jahr</small></div><div class='metric-card'><h3>Langzeittrend</h3><div id='trendText'>--</div><small>auf Basis der letzten vollständigen Jahre</small></div></div></div>

<div class='card'><h2>Verlaufsdaten</h2><div class='linkrow'><a class='btn' href='/history/import'>📥 CSV importieren</a><a class='btn' href='/history.csv?days=183'>CSV ½ Jahr</a><a class='btn' href='/history.csv?days=365'>CSV 1 Jahr</a><a class='btn' href='/history.csv?days=1825'>CSV 5 Jahre</a><a class='btn' href='/history.csv?days=3650'>CSV 10 Jahre</a></div></div>)HTML";
  html+=R"HTML(<script>(function(){const c=document.getElementById('hc'),t=document.getElementById('ht');let days=365,items=[],g=null;const f=v=>Number.isFinite(Number(v))?Number(v).toFixed(1):'--';function draw(){const r=c.getBoundingClientRect(),w=Math.max(320,Math.floor(r.width)),h=Math.floor(r.height),dpr=Math.max(1,devicePixelRatio||1);c.width=w*dpr;c.height=h*dpr;const x=c.getContext('2d');x.setTransform(dpr,0,0,dpr,0,0);x.clearRect(0,0,w,h);if(!items.length)return;const pl=42,pr=10,pt=15,pb=28,iw=w-pl-pr,ih=h-pt-pb,t0=items[0].time;let t1=items[items.length-1].time;if(t1<=t0)t1=t0+86400000;const px=z=>pl+(z-t0)/(t1-t0)*iw,py=p=>pt+ih-Math.max(0,Math.min(100,p))/100*ih;x.strokeStyle='#333';[0,25,50,75,100].forEach(v=>{let y=py(v);x.beginPath();x.moveTo(pl,y);x.lineTo(w-pr,y);x.stroke();x.fillStyle='#777';x.font='10px Arial';x.fillText(v+'%',4,y+3)});const sampleDays=days>365?Math.max(1,Math.ceil(days/400)):1,maxLineGap=Math.max(129600000,sampleDays*86400000*1.75);x.beginPath();items.forEach((a,i)=>{let q=px(a.time),y=py(a.percent),gap=i?(a.time-items[i-1].time):0;(i&&gap<=maxLineGap)?x.lineTo(q,y):x.moveTo(q,y)});x.strokeStyle='#4da6ff';x.lineWidth=2;x.stroke();let mc=Math.max(1,...items.map(a=>a.consumedLiters||0));items.forEach(a=>{let q=px(a.time),bh=(a.consumedLiters/mc)*ih*.32;x.fillStyle='#ffb52e';x.fillRect(q-1,pt+ih-bh,2,bh);const src=Number(a.source)||0;x.fillStyle=src===1?'#ffd166':(src===2?'#ff6b6b':'#4da6ff');x.beginPath();if(src===1){x.arc(q,py(a.percent),4,0,Math.PI*2);x.strokeStyle=x.fillStyle;x.lineWidth=2;x.stroke()}else if(src===2){x.moveTo(q,py(a.percent)-4);x.lineTo(q+4,py(a.percent)+4);x.lineTo(q-4,py(a.percent)+4);x.closePath();x.fill()}else{x.arc(q,py(a.percent),2.7,0,Math.PI*2);x.fill()}if(a.refillLiters>0){x.fillStyle='#42d65b';x.beginPath();x.arc(q,pt+ih-5,5,0,Math.PI*2);x.fill()}});g={t0,t1,px}}
c.onmousemove=e=>{if(!g||!items.length)return;let mx=e.offsetX,best=null,bd=1e9;items.forEach(a=>{let q=Math.abs(g.px(a.time)-mx);if(q<bd){bd=q;best=a}});if(!best||bd>25){t.style.display='none';return}let d=new Date(best.time);const srcName=Number(best.source)===1?'Import':(Number(best.source)===2?'Test':'Gemessen');t.innerHTML='<b>'+d.toLocaleDateString('de-DE')+'</b><br>Füllstand: '+f(best.percent)+' %<br>'+f(best.liters)+' L<br>Verbrauch: '+f(best.consumedLiters)+' L<br>Quelle: '+srcName+(best.refillLiters>0?'<br><span style="color:#42d65b">Nachfüllung: +'+f(best.refillLiters)+' L</span>':'');t.style.display='block';t.style.left=Math.min(c.clientWidth-185,Math.max(5,mx+12))+'px';t.style.top='10px'};c.onmouseleave=()=>t.style.display='none';async function loadRecentRefills(){const box=document.getElementById('recentRefills');try{const r=await fetch('/api/recent-refills?ts='+Date.now(),{cache:'no-store'});if(!r.ok)throw new Error('HTTP '+r.status);const j=await r.json();const a=j.items||[];if(!a.length){box.innerHTML='<p class="muted">Keine Nachfüllvorgänge vorhanden.</p>';return}let h='<table><tr><th>Datum</th><th>Menge</th><th>Füllstand danach</th></tr>';a.forEach(v=>{h+='<tr><td>'+v.date+'</td><td>+'+f(v.liters)+' L</td><td>'+((v.percent===null||v.percent===undefined)?'--':f(v.percent)+' %')+'</td></tr>'});h+='</table>';box.innerHTML=h}catch(e){box.innerHTML='<p class="muted">Nachfüllvorgänge konnten nicht geladen werden.</p>'}};async function load(n){days=n;document.querySelectorAll('.periodBtn').forEach(a=>a.classList.toggle('active',Number(a.dataset.days)===days));try{let r=await fetch('/api/history?days='+days+'&ts='+Date.now(),{cache:'no-store'});if(!r.ok)throw new Error('HTTP '+r.status);let raw=await r.text(),j;try{j=JSON.parse(raw)}catch(e){throw new Error('Ungültiges JSON: '+raw.substring(0,160))}items=j.items||[];document.getElementById('sd').textContent=days+' Tage';const z=j.stats||{};document.getElementById('sc').textContent=f(z.consumptionLiters)+' L';document.getElementById('sr').textContent=f(z.refillLiters)+' L';document.getElementById('sl').textContent=(Number.isFinite(Number(z.minPercent))&&Number.isFinite(Number(z.maxPercent)))?f(z.minPercent)+'–'+f(z.maxPercent)+' %':'--';draw()}catch(e){items=[];draw();document.getElementById('sd').textContent='Fehler';document.getElementById('sc').textContent='--';document.getElementById('sr').textContent='--';document.getElementById('sl').textContent=e.message}}function drawMonthChart(){const md=window.__monthData;if(!md)return;const c=document.getElementById('monthChart'),tip=document.getElementById('monthTip'),leg=document.getElementById('monthLegend');if(!c)return;const n=Math.max(1,Math.min(10,Number(document.getElementById('yearCount')?.value||5))),ys=md.ys.slice(-n),r=c.getBoundingClientRect(),w=Math.max(360,Math.floor(r.width)),h=Math.max(280,Math.floor(r.height)),dpr=Math.max(1,devicePixelRatio||1);c.width=w*dpr;c.height=h*dpr;const x=c.getContext('2d');x.setTransform(dpr,0,0,dpr,0,0);x.clearRect(0,0,w,h);const pl=46,pr=12,pt=18,pb=36,iw=w-pl-pr,ih=h-pt-pb;let maxV=1;for(const y of ys)for(let m=1;m<=12;m++)maxV=Math.max(maxV,md.net(md.monthly[y]?.[m]));const gy=v=>pt+ih-(v/maxV)*ih;x.strokeStyle='#333';x.fillStyle='#777';x.font='10px Arial';for(let i=0;i<=4;i++){const v=maxV*i/4,y=gy(v);x.beginPath();x.moveTo(pl,y);x.lineTo(w-pr,y);x.stroke();x.fillText(Math.round(v)+' L',4,y+3)}const groupW=iw/12,barGap=2,barW=Math.max(2,(groupW-8)/Math.max(1,ys.length));const palette=['#4da6ff','#ffb52e','#42d65b','#c77dff','#ff6b6b','#00c2d1','#ffd166','#8ac926','#f72585','#9aa0a6'];const rects=[];for(let m=1;m<=12;m++){const gx=pl+(m-1)*groupW;x.fillStyle='#aaa';x.font='10px Arial';x.fillText(md.names[m],gx+2,h-10);ys.forEach((y,j)=>{const v=md.net(md.monthly[y]?.[m]),bh=(v/maxV)*ih,bx=gx+4+j*barW,by=pt+ih-bh;x.fillStyle=palette[j%palette.length];x.fillRect(bx,by,Math.max(1,barW-barGap),bh);rects.push({x:bx,y:by,w:Math.max(1,barW-barGap),h:bh,year:y,month:md.names[m],value:v,color:palette[j%palette.length]})})}leg.innerHTML=ys.map((y,j)=>'<span style="display:inline-flex;align-items:center;gap:6px;margin:4px 14px 4px 0"><span style="display:inline-block;width:18px;height:12px;background:'+palette[j%palette.length]+';border-radius:2px;border:1px solid rgba(255,255,255,.35)"></span><b>Jahr '+y+'</b></span>').join('');c.onmousemove=e=>{const rr=c.getBoundingClientRect(),mx=e.clientX-rr.left,my=e.clientY-rr.top;let hit=null;for(const q of rects){if(mx>=q.x&&mx<=q.x+q.w&&my>=q.y&&my<=q.y+q.h){hit=q;break}}if(!hit){tip.style.display='none';return}tip.innerHTML='<b>'+hit.month+' '+hit.year+'</b><br>Verbrauch: '+f(hit.value)+' L';tip.style.display='block';tip.style.left=Math.min(c.clientWidth-170,Math.max(5,mx+12))+'px';tip.style.top=Math.max(5,my-20)+'px'};c.onmouseleave=()=>tip.style.display='none'}document.getElementById('yearCount')?.addEventListener('change',drawMonthChart);async function loadAnalysis(){try{const r=await fetch('/history.csv?days=3650&ts='+Date.now(),{cache:'no-store'});if(!r.ok)throw new Error('HTTP '+r.status);const raw=await r.text(),lines=raw.split(/\r?\n/).filter(Boolean),a=[];for(let i=1;i<lines.length;i++){const p=lines[i].split(';');if(p.length<5)continue;const dm=p[0].split('.');if(dm.length!==3)continue;const d=new Date(Number(dm[2]),Number(dm[1])-1,Number(dm[0]),12,0,0);const liters=Number(String(p[1]).replace(',','.')),cons=Number(String(p[3]).replace(',','.')),refill=Number(String(p[4]).replace(',','.'));if(!Number.isFinite(liters)||!Number.isFinite(d.getTime()))continue;a.push({time:d.getTime(),liters,consumedLiters:Number.isFinite(cons)?cons:0,refillLiters:Number.isFinite(refill)?refill:0})}a.sort((x,y)=>x.time-y.time);const monthly={},yearly={},years=new Set(),daysPerYear={};function mk(){return{cons:0,refill:0,days:0}}function add(q,v){const end=Number(v.liters),rf=Math.max(0,Number(v.refillLiters)||0),dc=Math.max(0,Number(v.consumedLiters)||0);if(!Number.isFinite(end))return;q.cons+=dc;q.refill+=rf;q.days++}let prevEnd=null;for(const v of a){const d=new Date(v.time),y=d.getFullYear(),m=d.getMonth()+1;years.add(y);daysPerYear[y]=(daysPerYear[y]||0)+1;monthly[y]??={};monthly[y][m]??=mk();yearly[y]??=mk();add(monthly[y][m],v);add(yearly[y],v);const e=Number(v.liters);if(Number.isFinite(e))prevEnd=e}function net(q){return q&&q.days?Math.max(0,Number(q.cons)||0):0}const now=new Date(),cy=now.getFullYear(),cm=now.getMonth()+1;document.getElementById('amCons').textContent=f(net(monthly[cy]?.[cm]))+' L';document.getElementById('ayCons').textContent=f(net(yearly[cy]))+' L';let all=mk();for(const v of a){add(all,v);}document.getElementById('avgDay').textContent=f(all.days?net(all)/all.days:0)+' L';function ytd(year){let q=mk();for(const v of a){const d=new Date(v.time);if(d.getFullYear()===year&&d.getMonth()<=now.getMonth()&&(d.getMonth()<now.getMonth()||d.getDate()<=now.getDate()))add(q,v)}return net(q)}const curYtd=ytd(cy),prevYtd=ytd(cy-1);document.getElementById('prevYearCons').textContent=f(prevYtd)+' L';document.getElementById('yearDelta').textContent=prevYtd>0?((curYtd-prevYtd)/prevYtd*100).toFixed(1)+' %':'--';const ys=[...years].sort((a,b)=>a-b),complete=ys.filter(y=>y<cy&&(daysPerYear[y]||0)>=330).slice(-4);let trend='--';if(complete.length>=2){const first=net(yearly[complete[0]]),last=net(yearly[complete[complete.length-1]]);if(first>0){const pct=(last-first)/first*100;trend=(pct>5?'steigend':(pct<-5?'fallend':'stabil'))+' ('+(pct>=0?'+':'')+pct.toFixed(1)+' %)'}}document.getElementById('trendText').textContent=trend;const names=['','Jan','Feb','Mär','Apr','Mai','Jun','Jul','Aug','Sep','Okt','Nov','Dez'];let h='<tr><th>Monat</th>'+ys.map(y=>'<th>'+y+'</th>').join('')+'</tr>';for(let m=1;m<=12;m++){h+='<tr><th>'+names[m]+'</th>'+ys.map(y=>'<td>'+f(net(monthly[y]?.[m]))+'</td>').join('')+'</tr>'}h+='<tr><th>Jahr gesamt</th>'+ys.map(y=>'<td><b>'+f(net(yearly[y]))+'</b></td>').join('')+'</tr>';document.getElementById('monthCompare').innerHTML=h;window.__monthData={ys,monthly,net,names};drawMonthChart()}catch(e){document.getElementById('monthCompare').innerHTML='<tr><td>Analyse konnte nicht geladen werden: '+e.message+'</td></tr>'}}document.querySelectorAll('.periodBtn').forEach(a=>a.onclick=e=>{e.preventDefault();load(Number(a.dataset.days))});load(365);loadRecentRefills();loadAnalysis();addEventListener('resize',()=>{draw();drawMonthChart()})})();</script>)HTML";
  html+=pageFooter();sendLocalizedHtml(200, html);
}

void handleRecentRefills() {
  // Die bestehenden Nachfüllereignisse werden bereits chronologisch
  // mit dem neuesten Ereignis an Position 0 gehalten.
  // Für den Füllstand danach suchen wir den passenden Tageswert im
  // vorhandenen internen RAM-Cache. Das vermeidet einen zusätzlichen
  // großen Historienpuffer.
  String json;
  json.reserve(1800);
  json = "{\"items\":[";
  bool first = true;
  uint8_t shown = 0;
  const float cap = calculateTankCapacityLiters();

  for (uint8_t i = 0; i < refillEventCount && shown < 5; ++i) {
    if (refillEvents[i].timestamp == 0 || !isfinite(refillEvents[i].liters)) continue;

    time_t ts = (time_t)refillEvents[i].timestamp;
    struct tm tmv = {};
    if (localtime_r(&ts, &tmv) == nullptr) continue;

    uint32_t dayKey = (uint32_t)(tmv.tm_year + 1900) * 10000UL +
                      (uint32_t)(tmv.tm_mon + 1) * 100UL +
                      (uint32_t)tmv.tm_mday;

    float litersAfter = NAN;
    for (uint16_t n = 0; n < historyCount && n < HISTORY_RAM_CACHE_DAYS; ++n) {
      if (historyStore[n].dayKey == dayKey && isfinite(historyStore[n].endLiters)) {
        litersAfter = historyStore[n].endLiters;
        break;
      }
    }

    if (!isfinite(litersAfter)) {
      HistoryEntry stored{};
      if (historyReadDayFromSpiffs(dayKey, stored) && isfinite(stored.endLiters)) {
        litersAfter = stored.endLiters;
      }
    }

    float percentAfter = NAN;
    if (isfinite(cap) && cap > 0.0f && isfinite(litersAfter)) {
      percentAfter = constrain(litersAfter / cap * 100.0f, 0.0f, 100.0f);
    }

    if (!first) json += ",";
    first = false;
    json += "{\"date\":\"";
    json += jsonEscape(formatRefillDate(refillEvents[i].timestamp));
    json += "\",\"timestamp\":";
    json += String(refillEvents[i].timestamp);
    json += ",\"liters\":";
    json += jsonFloat(refillEvents[i].liters, 1);
    json += ",\"percent\":";
    if (isfinite(percentAfter)) json += jsonFloat(percentAfter, 1);
    else json += "null";
    json += "}";
    ++shown;
  }

  json += "]}";
  server->sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server->sendHeader("Pragma", "no-cache");
  server->send(200, "application/json; charset=utf-8", json);
}

void handleHistory() {
  uint16_t requestedDays = 30;
  if (server->hasArg("days")) {
    requestedDays = (uint16_t)server->arg("days").toInt();
    if (requestedDays < 1) requestedDays = 30;
    if (requestedDays > HISTORY_DAYS) requestedDays = HISTORY_DAYS;
  }

  server->sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server->sendHeader("Pragma", "no-cache");
  server->setContentLength(CONTENT_LENGTH_UNKNOWN);
  server->send(200, "application/json; charset=utf-8", "");

  String chunk;
  chunk.reserve(2200);
  chunk = "{\"days\":";
  chunk += String(requestedDays);
  chunk += ",\"items\":[";
  server->sendContent(chunk);
  chunk = "";

  bool first = true;
  float totalConsumption = 0.0f;
  float totalRefill = 0.0f;
  float minPct = NAN;
  float maxPct = NAN;
  uint32_t actualDays = 0;

  auto flushChunk = [&]() {
    if (!chunk.isEmpty()) {
      server->sendContent(chunk);
      chunk = "";
      yield();
    }
  };

  auto addStats = [&](float pct, float cons, float refill) {
    if (isfinite(cons)) totalConsumption += max(0.0f, cons);
    if (isfinite(refill)) totalRefill += max(0.0f, refill);
    if (isfinite(pct)) {
      if (!isfinite(minPct) || pct < minPct) minPct = pct;
      if (!isfinite(maxPct) || pct > maxPct) maxPct = pct;
    }
  };

  auto appendItem = [&](uint32_t dayKey, float pct, float liters,
                        float cons, float refill, uint16_t source,
                        bool live) {
    int y = dayKey / 10000;
    int m = (dayKey / 100) % 100;
    int d = dayKey % 100;
    struct tm tmv = {};
    tmv.tm_year = y - 1900;
    tmv.tm_mon = m - 1;
    tmv.tm_mday = d;
    tmv.tm_hour = 12;
    tmv.tm_isdst = -1;
    const time_t sec = mktime(&tmv);
    if (sec == (time_t)-1) return;
    const uint64_t ts = (uint64_t)sec * 1000ULL;

    if (!first) chunk += ",";
    first = false;
    chunk += "{\"time\":";
    chunk += String(ts);
    chunk += ",\"dayKey\":";
    chunk += String(dayKey);
    chunk += ",\"percent\":";
    chunk += jsonFloat(pct, 1);
    chunk += ",\"liters\":";
    chunk += jsonFloat(liters, 1);
    chunk += ",\"consumedLiters\":";
    chunk += jsonFloat(cons, 1);
    chunk += ",\"refillLiters\":";
    chunk += jsonFloat(refill, 1);
    chunk += ",\"source\":";
    chunk += String(source);
    if (live) chunk += ",\"live\":true";
    chunk += "}";

    if (chunk.length() >= 1800) flushChunk();
  };

  const char *path = nullptr;
  HistoryFileHeader h{};
  const uint32_t todayKey = historyDayKeyNow();

  if (historyGetActiveFile(&path, &h)) {
    const uint32_t refDay = todayKey ? todayKey : h.lastDayKey;
    const uint32_t firstDay = refDay
      ? historyDayKeyAddDays(refDay, -((int32_t)requestedDays - 1))
      : 0;

    // Ausduennung richtet sich nach Kalendertagen, nicht nach der Anzahl
    // gespeicherter Datensaetze. Fehlende Tage verursachen dadurch weder
    // Zusatzschleifen noch falsche Zeitfenster.
    const uint32_t maxChartPoints = (requestedDays > 365) ? 400UL : 365UL;
    const uint32_t stepDays = max(1UL, ((uint32_t)requestedDays + maxChartPoints - 1) / maxChartPoints);
    const int32_t firstOffset = firstDay ? historyDayOffset(h.baseDay, firstDay) : 0;

    File f = SPIFFS.open(path, FILE_READ);
    if (f && f.seek(sizeof(HistoryFileHeader), SeekSet)) {
      const uint32_t count = min((uint32_t)h.count, (uint32_t)HISTORY_DAYS);
      for (uint32_t i = 0; i < count; ++i) {
        CompactHistoryDay item{};
        bool present = false;
        if (!historyReadAnyCompactRecord(f, h.version, i, item, present)) break;

        // Bei einer lueckenhaften Historie werden leere Kalenderslots ignoriert.
        if (!present) continue;

        const uint32_t dayKey = historyDayKeyAddDays(h.baseDay, item.dayOffset);
        if (!dayKey) continue;
        if (firstDay && dayKey < firstDay) continue;
        if (refDay && dayKey > refDay) continue;

        const float liters = (float)item.levelLiters;
        const float cons = (float)item.consumptionLiters;
        float refill = (float)item.refillLiters;
        const uint16_t source = item.source <= HISTORY_TEST ? item.source : HISTORY_MEASURED;

        const float cap = calculateTankCapacityLiters();

        // Defensive Plausibilisierung auch beim Lesen bereits vorhandener Dateien.
        if(isfinite(cap) && cap > 0.0f && refill > cap) refill = 0.0f;
        const float pct = (isfinite(cap) && cap > 0.0f)
          ? constrain(liters / cap * 100.0f, 0.0f, 100.0f)
          : NAN;

        actualDays++;
        addStats(pct, cons, refill);

        int32_t rel = (int32_t)item.dayOffset - firstOffset;
        if (rel < 0) rel = 0;
        const bool emit =
          ((uint32_t)rel % stepDays == 0) ||
          (refill > 0.0f) ||
          (dayKey == h.lastDayKey);

        if (emit) appendItem(dayKey, pct, liters, cons, refill, source, false);

        if ((i & 0x7F) == 0) yield();
      }
      f.close();
    } else if (f) {
      f.close();
    }
  }

  // Laufenden Tag nur ergaenzen, wenn er nicht bereits persistent vorhanden ist.
  if (todayKey && isfinite(tankLiters) && tankLiters >= 0.0f) {
    bool already = false;
    for (uint16_t i = 0; i < historyCount && i < HISTORY_RAM_CACHE_DAYS; ++i) {
      if (historyStore[i].dayKey == todayKey) {
        already = true;
        break;
      }
    }

    if (!already) {
      const float cap = calculateTankCapacityLiters();
      const float pct = (isfinite(cap) && cap > 0.0f)
        ? constrain(tankLiters / cap * 100.0f, 0.0f, 100.0f)
        : NAN;
      const float liveConsumption = max(0.0f, consumptionToday());
      const float liveRefill = max(0.0f, historyPendingRefill);

      addStats(pct, liveConsumption, liveRefill);
      actualDays++;
      appendItem(todayKey, pct, tankLiters, liveConsumption, liveRefill,
                 HISTORY_MEASURED, true);
    }
  }

  flushChunk();
  chunk = "]";
  chunk += ",\"stats\":{\"days\":";
  chunk += String(actualDays);
  chunk += ",\"consumptionLiters\":";
  chunk += jsonFloat(totalConsumption, 1);
  chunk += ",\"refillLiters\":";
  chunk += jsonFloat(totalRefill, 1);
  chunk += ",\"minPercent\":";
  chunk += jsonFloat(minPct, 1);
  chunk += ",\"maxPercent\":";
  chunk += jsonFloat(maxPct, 1);
  chunk += "}}";
  server->sendContent(chunk);
  server->sendContent("");
}


void handleHistoryCsv(){
  uint16_t days=3650;
  if(server->hasArg("days")){
    days=(uint16_t)server->arg("days").toInt();
    if(days<1)days=3650;
    if(days>HISTORY_DAYS)days=HISTORY_DAYS;
  }

  const char*path=nullptr;
  HistoryFileHeader h{};
  if(!historyGetActiveFile(&path,&h)){
    server->send(500,"text/plain; charset=utf-8","Historie nicht verfügbar");
    return;
  }

  File f=SPIFFS.open(path,FILE_READ);
  if(!f || !f.seek(sizeof(HistoryFileHeader),SeekSet)){
    if(f)f.close();
    server->send(500,"text/plain; charset=utf-8","Historiedatei konnte nicht gelesen werden");
    return;
  }

  const uint32_t todayKey=historyDayKeyNow();
  const uint32_t refDay=todayKey?todayKey:h.lastDayKey;
  const uint32_t firstDay=refDay?historyDayKeyAddDays(refDay,-((int32_t)days-1)):0;

  server->setContentLength(CONTENT_LENGTH_UNKNOWN);
  server->send(200,"text/csv; charset=utf-8","");
  server->sendContent("Datum;Fuellstand_L;Fuellstand_%25;Verbrauch_L;Nachfuellung_L;Quelle\r\n");

  String out;
  out.reserve(1200);
  const float cap=calculateTankCapacityLiters();
  const uint32_t count=min((uint32_t)h.count,(uint32_t)HISTORY_DAYS);

  for(uint32_t i=0;i<count;i++){
    CompactHistoryDay item{};
    bool present=false;
    if(!historyReadAnyCompactRecord(f,h.version,i,item,present))break;
    if(!present)continue;

    const uint32_t dayKey=historyDayKeyAddDays(h.baseDay,item.dayOffset);
    if(!dayKey)continue;
    if(firstDay&&dayKey<firstDay)continue;
    if(refDay&&dayKey>refDay)continue;

    const float liters=(float)item.levelLiters;
    const float cons=(float)item.consumptionLiters;
    const float refill=(float)item.refillLiters;
    const uint16_t source=item.source<=HISTORY_TEST?item.source:HISTORY_MEASURED;
    const float pct=(isfinite(cap)&&cap>0)
      ?constrain(liters/cap*100.0f,0.0f,100.0f):0.0f;

    char dateBuf[16];
    snprintf(dateBuf,sizeof(dateBuf),"%02u.%02u.%04u",
             (unsigned)(dayKey%100),
             (unsigned)((dayKey/100)%100),
             (unsigned)(dayKey/10000));

    out+=dateBuf;out+=';';
    out+=String(liters,1);out+=';';
    out+=String(pct,1);out+=';';
    out+=String(cons,1);out+=';';
    out+=String(refill,1);out+=';';
    out+=String(source);out+="\r\n";

    if(out.length()>1200){
      server->sendContent(out);
      out="";
      yield();
    }
  }
  f.close();
  if(out.length())server->sendContent(out);
  server->sendContent("");
}


// ============================================================================
// HISTORIEN-IMPORT
// ============================================================================

#define HISTORY_IMPORT_FILE "/history_import.csv"
#define HISTORY_IMPORT_MAX_PREVIEW 200

static bool importDigitsOnly(const String &v) {
  if(v.length()==0)return false;
  for(size_t i=0;i<v.length();++i) if(v[i]<'0'||v[i]>'9') return false;
  return true;
}

static bool parseImportDate(const String &input, uint32_t &dayKey) {
  String v=input; v.trim();
  int p1=v.indexOf('.');
  char sep='.';
  if(p1<0){p1=v.indexOf('-');sep='-';}
  if(p1<0)return false;
  int p2=v.indexOf(sep,p1+1); if(p2<0)return false;
  String sd=v.substring(0,p1), sm=v.substring(p1+1,p2), sy=v.substring(p2+1);
  sd.trim();sm.trim();sy.trim();
  int d=sd.toInt(), m=sm.toInt(), y=sy.toInt();
  if(!importDigitsOnly(sd)||!importDigitsOnly(sm)||!importDigitsOnly(sy))return false;
  if(d<1||d>31||m<1||m>12)return false;
  if(sy.length()==2)y=2000+y;
  if(y<2000||y>2099)return false;
  uint32_t today=historyDayKeyNow();
  const uint32_t candidate=(uint32_t)y*10000UL+(uint32_t)m*100UL+(uint32_t)d;
  if(today){
    if(candidate>today)return false;
    const uint32_t oldestAllowed=historyDayKeyAddDays(today,-(HISTORY_DAYS-1));
    if(oldestAllowed && candidate<oldestAllowed)return false;
  }
  dayKey=candidate;
  return true;
}

static bool parseImportValue(String v, bool percentMode, float offset, float &litersOut) {
  v.trim();
  v.replace(",", ".");
  if(v.length()==0)return false;
  char *endPtr=nullptr;
  float value=strtof(v.c_str(),&endPtr);
  if(endPtr==v.c_str()||*endPtr!='\0'||!isfinite(value))return false;
  value += offset;
  float cap=calculateTankCapacityLiters();
  if(!isfinite(cap)||cap<=0)return false;
  if(percentMode){
    if(value<0.0f||value>100.0f)return false;
    litersOut=cap*value/100.0f;
  }else{
    if(value<0.0f||value>cap)return false;
    litersOut=value;
  }
  return isfinite(litersOut)&&litersOut>=0.0f&&litersOut<=cap;
}

struct HistoryImportRow { uint32_t dayKey; float liters; };
static HistoryImportRow historyImportPreview[HISTORY_IMPORT_MAX_PREVIEW];

static uint16_t parseImportPreview(bool percentMode, float offset,
                                    uint32_t &valid, uint32_t &invalid,
                                    uint32_t &duplicates) {
  valid=invalid=duplicates=0;
  if(!historyEnsureSpiffs()) return 0;
  File f=SPIFFS.open(HISTORY_IMPORT_FILE,FILE_READ); if(!f)return 0;

  uint16_t previewCount=0;
  String line;
  uint32_t lineNo=0;
  while(f.available()){
    line=f.readStringUntil('\n'); line.replace("\r",""); line.trim();
    if(line.length()==0) continue;
    ++lineNo;
    if(lineNo==1 && (line.indexOf("Datum")>=0 || line.indexOf("datum")>=0)) continue;

    int sep=line.indexOf(';');
    if(sep<0){invalid++;continue;}
    String date=line.substring(0,sep), value=line.substring(sep+1);
    int extra=value.indexOf(';'); if(extra>=0)value=value.substring(0,extra);

    uint32_t day=0; float liters=0;
    if(!parseImportDate(date,day)||!parseImportValue(value,percentMode,offset,liters)){
      invalid++; continue;
    }
    valid++;

    // Vorschau speichert nur die ersten N gueltigen Datensaetze.
    // Die eigentliche Importfunktion liest die Datei spaeter erneut und
    // verarbeitet alle Zeilen direkt im gemeinsamen Arbeitsbereich.
    bool duplicateInPreview=false;
    for(uint16_t i=0;i<previewCount;i++){
      if(historyImportPreview[i].dayKey==day){
        historyImportPreview[i].liters=liters;
        duplicateInPreview=true;
        break;
      }
    }
    if(duplicateInPreview) duplicates++;
    else if(previewCount<HISTORY_IMPORT_MAX_PREVIEW){
      historyImportPreview[previewCount++]={day,liters};
    }
  }
  f.close();
  return previewCount;
}

static bool loadHistoryIntoWorkspace(uint16_t &countOut, HistoryFileHeader &hOut) {
  countOut = 0;
  const char *path = nullptr;
  if(!historyGetActiveFile(&path, &hOut)) return false;

  File f = SPIFFS.open(path, FILE_READ);
  if(!f) return false;
  if(!f.seek(sizeof(HistoryFileHeader), SeekSet)) {
    f.close();
    return false;
  }

  const uint32_t physicalCount = min((uint32_t)hOut.count, (uint32_t)HISTORY_DAYS);

  for(uint32_t i=0; i<physicalCount && countOut<HISTORY_DAYS; ++i) {
    CompactHistoryDay item{};
    bool present = false;

    // Derselbe lückensichere Leser wie für API/CSV/Verbrauch:
    // vollständig leere Kalenderslots werden nicht als baseDay-Dubletten
    // in den Import-Arbeitsbereich übernommen.
    if(!historyReadAnyCompactRecord(f, hOut.version, i, item, present)) break;
    if(!present) continue;

    HistoryEntry &r = historyWorkspace.entries[countOut];
    memset(&r, 0, sizeof(r));
    r.dayKey = historyDayKeyAddDays(hOut.baseDay, item.dayOffset);
    r.endLiters = item.levelLiters;
    r.consumedLiters = item.consumptionLiters;
    r.refillLiters = item.refillLiters;
    r.source = item.source <= HISTORY_TEST ? item.source : HISTORY_MEASURED;

    if(r.dayKey && isfinite(r.endLiters)) ++countOut;

    if((i & 0x7F) == 0) yield();
  }

  f.close();
  return countOut > 0;
}

static bool parseImportFloatField(String v, float &out) {
  v.trim(); v.replace(",", ".");
  if(v.isEmpty()) return false;
  char *endPtr=nullptr; out=strtof(v.c_str(),&endPtr);
  return endPtr!=v.c_str() && *endPtr=='\0' && isfinite(out);
}

static uint16_t parseImportIntoWorkspace(bool percentMode, float offset,
                                          uint32_t &valid, uint32_t &invalid,
                                          uint32_t &duplicates,
                                          uint16_t &rowCount,
                                          bool &extendedDetected) {
  valid=invalid=duplicates=0; extendedDetected=false;
  if(!historyEnsureSpiffs()) return 0;
  File f=SPIFFS.open(HISTORY_IMPORT_FILE,FILE_READ); if(!f)return 0;
  String line; uint32_t lineNo=0;
  while(f.available()){
    line=f.readStringUntil('\n'); line.replace("\r",""); line.trim();
    if(line.length()==0) continue;
    ++lineNo;
    if(lineNo==1 && (line.indexOf("Datum")>=0 || line.indexOf("datum")>=0)) continue;

    String cols[6]; uint8_t colCount=0; int from=0;
    while(colCount<6){int p=line.indexOf(';',from); if(p<0){cols[colCount++]=line.substring(from);break;} cols[colCount++]=line.substring(from,p); from=p+1;}
    if(colCount<2){invalid++;continue;}
    uint32_t day=0; float liters=0;
    if(!parseImportDate(cols[0],day)||!parseImportValue(cols[1],percentMode,offset,liters)){invalid++;continue;}

    float cons=0.0f, refill=0.0f; uint8_t source=HISTORY_IMPORTED;
    bool extended=false;
    // Eigenes Exportformat: Datum;Fuellstand_L;Fuellstand_%25;Verbrauch_L;Nachfuellung_L;Quelle
    if(colCount>=4){float v=0;if(parseImportFloatField(cols[3],v)&&v>=0){cons=v;extended=true;}}
    if(colCount>=5){float v=0;if(parseImportFloatField(cols[4],v)&&v>=0){refill=v;extended=true;}}
    if(colCount>=6){String sv=cols[5];sv.trim();int si=sv.toInt();if(si>=HISTORY_MEASURED&&si<=HISTORY_TEST){source=(uint8_t)si;extended=true;}}
    if(extended) extendedDetected=true;
    valid++;

    int found=-1; for(uint16_t i=0;i<rowCount;i++) if(historyWorkspace.entries[i].dayKey==day){found=i;break;}
    HistoryEntry nr{day,liters,cons,refill,extended?source:HISTORY_IMPORTED};
    if(found>=0){historyWorkspace.entries[found]=nr;duplicates++;}
    else if(rowCount<HISTORY_DAYS){historyWorkspace.entries[rowCount++]=nr;}
    else invalid++;
  }
  f.close(); return rowCount;
}

static void sortHistoryWorkspace(uint16_t count){
  for(uint16_t i=1;i<count;i++){
    HistoryEntry key=historyWorkspace.entries[i];
    int j=(int)i-1;
    while(j>=0 && historyWorkspace.entries[j].dayKey>key.dayKey){
      historyWorkspace.entries[j+1]=historyWorkspace.entries[j];
      --j;
    }
    historyWorkspace.entries[j+1]=key;
  }
}

static uint32_t historyDayKeyToTimestamp(uint32_t dayKey) {
  if(dayKey == 0) return 0;
  struct tm t = {};
  t.tm_year = (int)(dayKey / 10000UL) - 1900;
  t.tm_mon  = (int)((dayKey / 100UL) % 100UL) - 1;
  t.tm_mday = (int)(dayKey % 100UL);
  t.tm_hour = 12;
  t.tm_isdst = -1;
  time_t ts = mktime(&t);
  return ts > 100000 ? (uint32_t)ts : 0;
}

static void rebuildRefillEventsFromHistory(const HistoryEntry *rows, uint16_t count) {
  clearRefillEvents();

  // Historie ist aufsteigend sortiert. Rueckwaerts laufen, damit Index 0
  // immer das neueste Ereignis bleibt.
  for(int i=(int)count-1; i>=0 && refillEventCount<REFILL_EVENT_COUNT; --i) {
    const HistoryEntry &r = rows[i];
    if(!r.dayKey || !isfinite(r.refillLiters) || r.refillLiters < REFILL_MIN_LITERS) continue;

    uint32_t ts = historyDayKeyToTimestamp(r.dayKey);
    if(ts == 0) continue;

    refillEvents[refillEventCount].timestamp = ts;
    refillEvents[refillEventCount].liters = r.refillLiters;
    ++refillEventCount;
  }

  if(refillEventCount > 0) {
    lastConfirmedRefillLiters = refillEvents[0].liters;
    lastRefillTimestamp = refillEvents[0].timestamp;
  } else {
    lastConfirmedRefillLiters = 0.0f;
    lastRefillTimestamp = 0;
  }
  confirmedRefillPendingLiters = 0.0f;
  saveRefillEvents();

  Serial.printf("[REFILL] Ereignisliste aus Historie neu aufgebaut: %u Ereignisse\n",
                (unsigned)refillEventCount);
}

static void recomputeHistoryDerived(HistoryEntry *rows, uint16_t count) {
  if(!rows || count == 0) return;

  for(uint16_t i=0;i<count;i++){
    rows[i].consumedLiters = 0.0f;
    rows[i].refillLiters = 0.0f;
    if(i==0) continue;

    // Nur direkt aufeinanderfolgende Kalendertage dürfen gegeneinander
    // ausgewertet werden. Datenlücken erzeugen weder Verbrauch noch Nachfüllung.
    const int32_t gapDays = historyDayOffset(rows[i-1].dayKey, rows[i].dayKey);
    if(gapDays != 1) continue;

    const float prev = rows[i-1].endLiters;
    const float curr = rows[i].endLiters;
    if(!isfinite(prev) || !isfinite(curr)) continue;

    const float delta = prev - curr;

    if(delta >= 0.0f) {
      // Normaler Verbrauch / gleichbleibender Stand.
      rows[i].consumedLiters = delta;
      continue;
    }

    const float rise = -delta;
    if(rise < REFILL_MIN_LITERS) {
      // Kleine positive Schwankung: weder Verbrauch noch Nachfüllung.
      continue;
    }

    // Ein großer positiver Sprung gilt nur dann als Nachfüllung, wenn das
    // erhöhte Niveau durch nachfolgende vorhandene Messungen bestätigt wird.
    // Damit werden einzelne Ausreißer / Messsprünge nicht als Betankung gewertet.
    bool confirmed = false;
    uint8_t confirmPoints = 0;

    // Toleranz: nach einer echten Nachfüllung darf der Tank bereits wieder
    // etwas Verbrauch zeigen. Als Bestätigung reicht ein Wert, der deutlich
    // über dem Vorfüllstand bleibt.
    const float confirmFloor = prev + rise * 0.50f;

    for(uint16_t j=i+1; j<count && confirmPoints<3; ++j) {
      const int32_t dg = historyDayOffset(rows[j-1].dayKey, rows[j].dayKey);
      if(dg != 1) break;  // Bestätigung nicht über eine neue Datenlücke hinweg.

      if(!isfinite(rows[j].endLiters)) break;

      ++confirmPoints;
      if(rows[j].endLiters >= confirmFloor) {
        confirmed = true;
      } else {
        // Fällt der Stand sofort wieder nahe auf das alte Niveau zurück,
        // war der Sprung sehr wahrscheinlich ein Ausreißer.
        confirmed = false;
        break;
      }
    }

    // Bei nur einem vorhandenen Folgetag darf auch dieser bereits bestätigen.
    if(confirmed) {
      rows[i].refillLiters = rise;
    }
  }
}
static void handleHistoryImportPage(bool resultPage=false, const String &message=String()) {
  String html=pageHeader("Historie importieren");
  html+="<div class='card'><h1>Historie importieren</h1>";
  html+="<p>CSV: <b>Datum;Wert</b>. Datum erlaubt z.B. 01.01.2026, 01-01-2026, 1.1.26 oder 1-1-26.</p>";
  html+="<p>Zweistellige Jahre werden immer als 2000+YY interpretiert. Zukünftige Daten und Daten außerhalb der letzten 10 Jahre werden verworfen. Tag 1-31 und Monat 1-12.</p>";
  html+="<form method='POST' action='/history/import' enctype='multipart/form-data'>";
  html+="<p><input type='file' name='historyFile' accept='.csv,text/csv' required></p>";
  html+="<p>Wert: <select name='type'><option value='liters'>Liter</option><option value='percent'>Füllstand %</option></select></p>";
  html+="<p>Offset: <input type='number' name='offset' step='0.1' value='0'> <small>Liter bzw. Prozentpunkte</small></p>";
  html+="<p><small>Nachfüllungen werden bei einfachem Datum/Liter-Import automatisch erkannt, aber nur bei einem großen Sprung zwischen direkt aufeinanderfolgenden Tagen, der durch nachfolgende Messwerte bestätigt wird. Datenlücken erzeugen keine Nachfüllung. Explizite Nachfüllspalten im Fuellstandsmesser-Export werden unverändert übernommen.</small></p>";
  html+="<p><button name='action' value='preview' type='submit'>Vorschau</button> <button name='action' value='import' type='submit'>Importieren</button></p></form>";
  if(resultPage&&!message.isEmpty())html+="<div class='card'><b>"+htmlEscape(message)+"</b></div>";
  html+="<p><a class='btn' href='/history'>Zur Historie</a></p></div>";
  html+=pageFooter(); sendLocalizedHtml(200, html);
}

void handleHistoryImportUpload(){
  HTTPUpload &upload=server->upload();
  if(upload.status==UPLOAD_FILE_START){
    if(historyEnsureSpiffs())SPIFFS.remove(HISTORY_IMPORT_FILE);
    File f=SPIFFS.open(HISTORY_IMPORT_FILE,FILE_WRITE); if(f)f.close();
  }else if(upload.status==UPLOAD_FILE_WRITE){
    File f=SPIFFS.open(HISTORY_IMPORT_FILE,FILE_APPEND); if(f){f.write(upload.buf,upload.currentSize);f.close();}
  }
}

void handleHistoryImport(){
  bool percentMode=server->arg("type")=="percent";
  float offset=server->hasArg("offset")?server->arg("offset").toFloat():0.0f;
  String action=server->arg("action");

  uint32_t valid=0,invalid=0,duplicates=0;
  if(action!="import"){
    uint16_t count=parseImportPreview(percentMode,offset,valid,invalid,duplicates);
    String html=pageHeader("Import-Vorschau");
    html+="<div class='card'><h1>Import-Vorschau</h1>";
    html+="<p>Gültig: <b>"+String(valid)+"</b> | Verworfen: <b>"+String(invalid)+"</b> | Duplikate: <b>"+String(duplicates)+"</b></p>";
    html+="<table><tr><th>Nr.</th><th>Datum</th><th>Liter</th><th>Status</th></tr>";
    uint16_t show=min((uint16_t)HISTORY_IMPORT_MAX_PREVIEW,count);
    for(uint16_t i=0;i<show;i++){
      html+="<tr><td>"+String(i+1)+"</td><td>"+String(historyImportPreview[i].dayKey)+"</td><td>"+String(historyImportPreview[i].liters,1)+"</td><td>gültig</td></tr>";
    }
    if(count>show)html+="<tr><td colspan='4'>... weitere gültige Datensätze</td></tr>";
    html+="</table><p>Beim Import gewinnt bei gleichem Datum der zuletzt gelesene gültige Datensatz und darf einen gemessenen Wert überschreiben. Nachfüllungen aus einfachen Datum/Liter-Daten benötigen eine bestätigte dauerhafte Niveauanhebung.</p>";
    html+="<p><a class='btn' href='/history/import'>Neue Datei</a> <a class='btn' href='/history'>Abbrechen</a></p></div>";
    html+=pageFooter(); sendLocalizedHtml(200, html); return;
  }

  // Zuerst die vorhandene Historie direkt in den gemeinsamen HistoryEntry-
  // Arbeitsbereich laden. Kein zweites 3650er Array erforderlich.
  uint16_t rowCount=0;
  HistoryFileHeader h{};
  loadHistoryIntoWorkspace(rowCount,h);

  // Danach CSV erneut zeilenweise einlesen und vorhandene Tageswerte direkt
  // ersetzen bzw. neue Werte anhaengen.
  bool extendedImport=false;
  rowCount=parseImportIntoWorkspace(percentMode,offset,valid,invalid,duplicates,rowCount,extendedImport);
  if(rowCount==0){
    handleHistoryImportPage(true,"Keine gültigen Datensätze gefunden."); return;
  }

  sortHistoryWorkspace(rowCount);

  Serial.printf("[IMPORT] Datensaetze=%u, extended=%s, Nachfuell-Erkennung=%s\n",
                (unsigned)rowCount,
                extendedImport ? "ja" : "nein",
                extendedImport ? "aus CSV" : "bestaetigte Spruenge");

  // Auf die letzten 3650 Tage begrenzen.
  if(rowCount>HISTORY_DAYS){
    uint16_t drop=rowCount-HISTORY_DAYS;
    memmove(historyWorkspace.entries,
            historyWorkspace.entries+drop,
            sizeof(HistoryEntry)*HISTORY_DAYS);
    rowCount=HISTORY_DAYS;
  }

  if(!extendedImport) recomputeHistoryDerived(historyWorkspace.entries,rowCount);

  // Zuerst den RAM-Cache aus dem noch unzerstoerten HistoryEntry-Bereich
  // aktualisieren.
  memcpy(historyStore,
         historyWorkspace.entries+(rowCount>HISTORY_RAM_CACHE_DAYS?rowCount-HISTORY_RAM_CACHE_DAYS:0),
         sizeof(HistoryEntry)*min(rowCount,(uint16_t)HISTORY_RAM_CACHE_DAYS));
  historyCount=min(rowCount,(uint16_t)HISTORY_RAM_CACHE_DAYS);
  historyIndex=(historyCount==HISTORY_RAM_CACHE_DAYS)?0:historyCount;
  historyLastDayKey=historyWorkspace.entries[rowCount-1].dayKey;
  historyStoreDirty=false;


  // Danach denselben RAM-Bereich als kompaktes SPIFFS-Format verwenden.
  // CompactHistoryDay ist kleiner als HistoryEntry; Vorwaertskonvertierung
  // ueberschreibt deshalb keine noch benoetigten Folgeeintraege.
  uint32_t base=historyWorkspace.entries[0].dayKey;
  const uint32_t lastImportedDay = historyWorkspace.entries[rowCount-1].dayKey;

  for(uint16_t i=0;i<rowCount;i++){
    HistoryEntry r=historyWorkspace.entries[i];
    historyCompactFromEntry(historyWorkspace.compact[i],r,base);
  }

  // WICHTIG: HistoryWorkspace ist eine Union. Hinter den gerade erzeugten
  // CompactHistoryDay-Einträgen liegen noch Bytes der vorherigen HistoryEntry-
  // Darstellung. Diese RAM-Restdaten dürfen niemals mit in SPIFFS geschrieben
  // werden, weil sie sonst später wie gültige dayOffset/refillLiters aussehen.
  if(rowCount < HISTORY_DAYS) {
    memset(historyWorkspace.compact + rowCount, 0,
           sizeof(CompactHistoryDay) * (HISTORY_DAYS - rowCount));
  }

  // Importdaten liegen zunächst sequentiell in compact[0..rowCount-1].
  // Für die persistente Datei auf das einheitliche dichte Kalenderlayout
  // umsetzen: Index == dayOffset, echte Datenlücken bleiben vollständig 0.
  historyNormalizeCompactLayout(historyWorkspace.compact, rowCount);

  int32_t spanDays = historyDayOffset(base, lastImportedDay) + 1;
  if(spanDays < 1) spanDays = 1;
  if(spanDays > HISTORY_DAYS) spanDays = HISTORY_DAYS;
  const uint16_t storedSpan = (uint16_t)spanDays;

  Serial.printf("[IMPORT] Persistenzlayout: %u Datensaetze / %u Kalendertage\n",
                (unsigned)rowCount, (unsigned)storedSpan);

  // Die Upload-CSV wird fuer den Commit nicht mehr benoetigt.
  // Vor dem Schreiben entfernen, damit ihr Speicherplatz sofort frei wird.
  SPIFFS.remove(HISTORY_IMPORT_FILE);

  if(!saveGeneratedCompactHistory(historyWorkspace.compact,storedSpan,base,
                                  historyLastDayKey,historyDayStartLiters,
                                  historyLastLiters,historyPendingRefill)){
    handleHistoryImportPage(true,"Import fehlgeschlagen: Historie wurde nicht gespeichert."); return;
  }

  historyStoreDirty=false;

  // Nach erfolgreichem Commit die Ereignisliste aus exakt der gerade
  // gespeicherten History rekonstruieren. Dadurch gibt es keine zweite,
  // möglicherweise veraltete Wahrheit in NVS.
  if(!rebuildRefillEventsFromPersistentHistory()) {
    clearRefillEvents();
    saveRefillEvents();
    Serial.println("[REFILL] WARNUNG: Ereignisliste nach Import nicht aus Historie lesbar");
  }

  SPIFFS.remove(HISTORY_IMPORT_FILE);
  String msg="Import erfolgreich: "+String(valid)+" gültig, "+String(invalid)+" verworfen, "+String(duplicates)+" Duplikate. ";
  if(extendedImport) {
    msg += "Export-Zusatzdaten (Verbrauch/Nachfüllung/Quelle) übernommen. ";
  } else {
    msg += "Verbrauch neu berechnet; Nachfüllungen nur aus bestätigten direkten Tages-Sprüngen abgeleitet. ";
  }
  msg += "Ereignisliste aus gespeicherter Historie neu aufgebaut. ";
  msg += "Letzter Datensatz gewinnt.";
  handleHistoryImportPage(true,msg);
}

// ============================================================================
// WEB VERBRAUCH
// ============================================================================

void handleConsumption() {
  String html = pageHeader("Verbrauch");

  html += "<div class='card'>";
  html += "<h1>Verbrauch</h1>";
  html += "<div class='grid'>";
  html += "<div class='metric-card'><h3>Heute</h3><div>";
  html += String(consumptionToday(), 1);
  html += " L</div></div>";
  html += "<div class='metric-card'><h3>Ø 7 Tage</h3><div>";
  html += String(consumptionAverage7Days(), 1);
  html += " L/Tag</div></div>";
  html += "<div class='metric-card'><h3>Ø 30 Tage</h3><div>";
  html += String(consumptionAverage30Days(), 1);
  html += " L/Tag</div></div>";
  html += "<div class='metric-card'><h3>Aktueller Monat</h3><div>";
  html += String(consumptionMonth(), 1);
  html += " L</div></div>";
  html += "</div>";
  html += "</div>";

  html += "<div class='card'>";
  html += "<h2>Nachfüllungen</h2>";
  if (refillEventCount == 0) {
    html += "<p>Noch keine Nachfüllung erkannt.</p>";
  } else {
    html += "<table><tr><th>Datum</th><th>Menge</th></tr>";
    for (uint8_t i = 0; i < refillEventCount; i++) {
      html += "<tr><td>";
      html += htmlEscape(formatRefillDate(refillEvents[i].timestamp));
      html += "</td><td>+";
      html += String(refillEvents[i].liters, 1);
      html += " L</td></tr>";
    }
    html += "</table>";
  }
  html += "</div>";

  html += "<div class='card'>";
  html += "<h2>Gespeicherte Tageswerte</h2>";
  html += "<p>Es werden nur Tageswerte gespeichert. Der Flash-Speicher wird "
          "dadurch deutlich weniger belastet als bei einer Speicherung jeder Messung.</p>";
  html += "<table><tr><th>Tag</th><th>Verbrauch</th><th>Nachfüllung</th></tr>";

  int idx = (int)historyIndex - (int)historyCount;
  while (idx < 0) idx += HISTORY_RAM_CACHE_DAYS;

  for (uint16_t n = 0; n < historyCount; n++) {
    const HistoryEntry &record = historyStore[idx];
    if (record.dayKey != 0) {
      html += "<tr><td>";
      html += String(record.dayKey);
      html += "</td><td>";
      html += String(record.consumedLiters, 1);
      html += " L</td><td>+";
      html += String(record.refillLiters, 1);
      html += " L</td></tr>";
    }
    if (++idx >= HISTORY_RAM_CACHE_DAYS) idx = 0;
  }

  html += "</table>";
  html += "</div>";

  html += pageFooter();
  sendLocalizedHtml(200, html);
}

// ============================================================================
// WEB FACTORY RESET
// ============================================================================

void handleFactoryReset() {

  factoryReset();

  String html =
    pageHeader("Factory Reset");

  html += "<div class='card'>";
  html += "<h1>Factory Reset</h1>";
  html += "<p>Werkseinstellungen wurden geladen.</p>";
  html += "<a class='btn' href='/reboot'>Neustart</a>";
  html += "</div>";

  html += pageFooter();

  server->send(
    200,
    "text/html; charset=utf-8",
    html
);
}

// ============================================================================
// WEB REBOOT
// ============================================================================

void handleReboot() {

  String html =
    pageHeader("Neustart");

  html += "<div class='card'>";
  html += "<h1>Neustart</h1>";
  html += "<p>ESP32 wird neu gestartet.</p>";
  html += "</div>";

  html += pageFooter();

  server->send(
    200,
    "text/html; charset=utf-8",
    html
  );

  delay(500);

  ESP.restart();
}

// ============================================================================
// WEB OTA UPDATE PAGE
// ============================================================================

void handleClearConsumptionData() {
  Serial.println("[HISTORY] Loesche alle Verbrauchs- und Historiedaten...");

  // RAM-Zustand vollstaendig zuruecksetzen.
  memset(historyStore, 0, sizeof(historyStore));
  historyIndex = 0;
  historyCount = 0;
  historyLastDayKey = 0;
  historyDayStartLiters = NAN;
  historyLastLiters = NAN;
  historyPendingRefill = 0.0f;
  historyStoreDirty = false;
  historyLastLiveSaveMs = 0;

  // Nachfuell-/Verbrauchsereignisse ebenfalls vollstaendig loeschen.
  clearRefillEvents();
  lastConfirmedRefillLiters = 0.0f;
  lastRefillTimestamp = 0;
  confirmedRefillPendingLiters = 0.0f;
  saveRefillEvents();

  // Eine leere Historie kann absichtlich nicht mit dem normalen
  // Historienformat gespeichert werden (kein baseDay). Deshalb beide Banken,
  // TMP-Dateien und eine eventuell verbliebene Importdatei direkt entfernen.
  bool saved = historyEnsureSpiffs();
  if (saved) {
    SPIFFS.remove(HISTORY_SPIFFS_FILE_A);
    SPIFFS.remove(HISTORY_SPIFFS_FILE_B);
    SPIFFS.remove(HISTORY_SPIFFS_TMP_A);
    SPIFFS.remove(HISTORY_SPIFFS_TMP_B);
    SPIFFS.remove(HISTORY_IMPORT_FILE);

    Preferences hm;
    if (hm.begin(HISTORY_NVS_META_NS, false)) {
      hm.clear();
      hm.end();
    } else {
      saved = false;
    }
  }

  Serial.printf(
    "[HISTORY] Verbrauchsdaten geloescht: %s, SPIFFS frei=%u Byte\n",
    saved ? "OK" : "FEHLER",
    historySpiffsReady ? (unsigned)(SPIFFS.totalBytes()-SPIFFS.usedBytes()) : 0U
  );

  String html = pageHeader("Verbrauchsdaten gelöscht");
  html += "<div class='card'>";
  html += "<h1>Verbrauchsdaten gelöscht</h1>";
  html += "<p>Alle gespeicherten Verbrauchs- und Historienwerte wurden gelöscht.</p>";
  html += "<p>Die aktuelle Sensor-/Füllstandsmessung bleibt unverändert.</p>";

  if (saved) {
    html += "<p><b>Persistenz: OK</b></p>";
  } else {
    html += "<p><b>Warnung: Speichern der gelöschten Historie fehlgeschlagen.</b></p>";
  }

  html += "<a class='btn' href='/'>Dashboard</a> ";
  html += "<a class='btn' href='/update'>Diagnose</a>";
  html += "</div>";
  html += pageFooter();

  sendLocalizedHtml(200, html);
}

static uint8_t requestedTestHistorySource() {
  if(!server->hasArg("source")) return HISTORY_TEST;
  String v=server->arg("source"); v.toLowerCase();
  if(v=="imported") return HISTORY_IMPORTED;
  if(v=="measured") return HISTORY_MEASURED;
  return HISTORY_TEST;
}

void handleGenerateTestHistory() {
  // SICHERER TESTDATENGENERATOR
  //
  // Sicherheitsprinzip:
  // 1. SPIFFS muss VOR jeder Änderung der Live-Historie verfügbar sein.
  //    Bei einem defekten Dateisystem wird es durch historyEnsureSpiffs() automatisch formatiert.
  // 2. Testdaten werden zunächst vollständig in einem statischen Puffer erzeugt.
  // 3. Die bestehende Live-Historie wird gesichert.
  // 4. Erst danach wird der RAM-Cache ersetzt und persistent gespeichert.
  // 5. Bei JEDEM Persistenzfehler wird der alte RAM-Zustand zurückgerollt.
  // 6. Nachfüll-Events werden ebenfalls erst nach erfolgreicher Historie gespeichert.

  if (!historyEnsureSpiffs()) {
    Serial.println("[HISTORY] Testdaten abgebrochen: SPIFFS nicht verfügbar");
    Serial.println("[HISTORY] Bestehende Historie bleibt unverändert");
    server->send(500, "text/plain; charset=utf-8",
                 "Testdaten abgebrochen: SPIFFS nicht verfuegbar. Bestehende Historie wurde nicht veraendert.");
    return;
  }

  time_t now = time(nullptr);
  if (now < 1000000000UL) {
    server->send(503, "text/plain; charset=utf-8",
                 "NTP-Zeit ist noch nicht synchronisiert. Bitte kurz warten und erneut versuchen.");
    return;
  }

  float capacity = calculateTankCapacityLiters();
  if (!isfinite(capacity) || capacity <= 0.0f) {
    server->send(500, "text/plain; charset=utf-8",
                 "Tankkapazitaet konnte nicht bestimmt werden.");
    return;
  }

  const uint16_t DAYS = HISTORY_RAM_CACHE_DAYS;
  const uint8_t refillCount = 5;
  const uint8_t generatedSource = requestedTestHistorySource();

  static const uint16_t refillDays[6] = {62, 126, 190, 248, 305, 340};
  static const float refillPct[6] = {0.20f, 0.16f, 0.24f, 0.18f, 0.22f, 0.15f};

  struct tm todayTm;
  localtime_r(&now, &todayTm);
  todayTm.tm_hour = 12;
  todayTm.tm_min = 0;
  todayTm.tm_sec = 0;
  time_t todayNoon = mktime(&todayTm);

  // Statischer Puffer: NICHT auf dem loopTask-Stack.
  static HistoryEntry generated[HISTORY_RAM_CACHE_DAYS];
  static HistoryEntry backup[HISTORY_RAM_CACHE_DAYS];
  memset(generated, 0, sizeof(generated));

  // Vollständiger RAM-Zustand vor dem Commit sichern.
  memcpy(backup, historyStore, sizeof(historyStore));
  const uint16_t oldHistoryIndex = historyIndex;
  const uint16_t oldHistoryCount = historyCount;
  const uint32_t oldHistoryLastDayKey = historyLastDayKey;
  const float oldHistoryDayStartLiters = historyDayStartLiters;
  const float oldHistoryLastLiters = historyLastLiters;
  const float oldHistoryPendingRefill = historyPendingRefill;
  const bool oldHistoryDirty = historyStoreDirty;

  float previousLiters = capacity * 0.90f;

  for (uint16_t i = 0; i < DAYS; ++i) {
    time_t dayTime = todayNoon - (time_t)(DAYS - 1 - i) * 86400;
    struct tm dayTm;
    localtime_r(&dayTime, &dayTm);

    float dayOfYear = (float)dayTm.tm_yday;
    float seasonal = 1.0f + 0.55f * cosf(((dayOfYear - 15.0f) / 365.0f) * 6.2831853f);
    float dailyVariation = 1.0f + 0.08f * sinf((float)i * 1.731f) + 0.04f * cosf((float)i * 0.417f);
    float dailyConsumption = capacity * 0.0060f * seasonal * dailyVariation;
    dailyConsumption = constrain(dailyConsumption, 0.001f, capacity * 0.025f);

    bool isRefill = false;
    uint8_t refillNo = 0;
    for (uint8_t r = 0; r < refillCount; ++r) {
      if (i == refillDays[r]) { isRefill = true; refillNo = r; break; }
    }

    float endLiters = previousLiters - dailyConsumption;
    float refillLiters = 0.0f;

    if (isRefill) {
      float amount = capacity * refillPct[refillNo] * 2.20f;
      amount *= 1.0f + ((int)(refillNo % 5) - 2) * 0.025f;
      float beforeRefill = previousLiters;
      endLiters = min(beforeRefill + amount, capacity * 0.95f);
      refillLiters = max(0.0f, endLiters - beforeRefill);
      dailyConsumption = 0.0f;
    }

    endLiters = constrain(endLiters, capacity * 0.05f, capacity * 0.96f);
    if (!isfinite(endLiters)) endLiters = previousLiters;

    HistoryEntry &entry = generated[i];
    entry.dayKey = (uint32_t)(dayTm.tm_year + 1900) * 10000UL +
                   (uint32_t)(dayTm.tm_mon + 1) * 100UL +
                   (uint32_t)dayTm.tm_mday;
    entry.endLiters = endLiters;
    entry.consumedLiters = dailyConsumption;
    entry.refillLiters = refillLiters;
    entry.source = generatedSource;
    previousLiters = endLiters;
  }

  // RAM-Cache jetzt erst für die Persistenz vorbereiten.
  memcpy(historyStore, generated, sizeof(generated));
  historyCount = DAYS;
  historyIndex = 0;
  historyLastDayKey = generated[DAYS - 1].dayKey;

  float liveLiters = (isfinite(tankLiters) && tankLiters >= 0.0f) ? tankLiters : generated[DAYS - 1].endLiters;
  historyDayStartLiters = liveLiters;
  historyLastLiters = liveLiters;
  historyPendingRefill = 0.0f;
  historyStoreDirty = true;

  // Persistenz zuerst. Bei Fehler wird alles zurückgerollt.
  bool saved = saveHistoryStore();
  if (!saved) {
    memcpy(historyStore, backup, sizeof(historyStore));
    historyIndex = oldHistoryIndex;
    historyCount = oldHistoryCount;
    historyLastDayKey = oldHistoryLastDayKey;
    historyDayStartLiters = oldHistoryDayStartLiters;
    historyLastLiters = oldHistoryLastLiters;
    historyPendingRefill = oldHistoryPendingRefill;
    historyStoreDirty = oldHistoryDirty;

    Serial.println("[HISTORY] Testdaten abgebrochen: Persistenz fehlgeschlagen");
    Serial.println("[HISTORY] Bestehende Historie wurde wiederhergestellt");
    server->send(500, "text/plain; charset=utf-8",
                 "Testdaten abgebrochen: Persistenz fehlgeschlagen. Bestehende Historie wurde wiederhergestellt.");
    return;
  }

  // Erst nach erfolgreicher Historienpersistenz Nachfüll-Events ersetzen.
  clearRefillEvents();
  refillEventCount = min(refillCount, (uint8_t)REFILL_EVENT_COUNT);
  for (uint8_t r = 0; r < refillEventCount; ++r) {
    // Vertrag im restlichen Programm: Index 0 = neuestes Ereignis.
    const uint8_t src = (uint8_t)(refillEventCount - 1 - r);
    uint16_t dayIndex = refillDays[src];
    time_t eventTime = todayNoon - (time_t)(DAYS - 1 - dayIndex) * 86400;
    refillEvents[r].timestamp = (uint32_t)eventTime;
    refillEvents[r].liters = generated[dayIndex].refillLiters;
  }

  if (refillEventCount > 0) {
    lastConfirmedRefillLiters = refillEvents[0].liters;
    lastRefillTimestamp = refillEvents[0].timestamp;
  } else {
    lastConfirmedRefillLiters = 0.0f;
    lastRefillTimestamp = 0;
  }
  confirmedRefillPendingLiters = 0.0f;
  saveRefillEvents();

  Serial.printf("[HISTORY] 1-Jahres-Testdaten erzeugt: %u Tage, Nachfuellungen=%u, Tank=%.0f L\n", DAYS, refillEventCount, capacity);
  Serial.println("[HISTORY] Saisonprofil: Winter hoch / Sommer niedrig / Uebergangszeiten mittel");
  Serial.println("[HISTORY] Testdaten: kleine taegliche Schwankungen, Nachfuellungen als Spruenge (+100%%)");
  Serial.println("[HISTORY] Persistenz-Verifikation: OK");

  String html = pageHeader("1 Jahr Testdaten erzeugt");
  html += "<div class='card'>";
  html += "<h1>1 Jahr Testdaten erzeugt</h1>";
  html += "<p>365 Tageswerte wurden erzeugt und erfolgreich persistent gespeichert.</p>";
  html += "<p>Testdaten skaliert auf eingestellte Tankgroesse: <b>"; html += String(capacity, 0); html += " L</b></p>";
  html += "<p>Nachfuellungen: <b>"; html += String(refillEventCount); html += "</b></p>";
  html += "<p>Verbrauch: <b>Winter höher, Sommer niedriger</b></p>";
  html += "<p>Mit kleinen täglichen Schwankungen und echten Nachfüllsprüngen.</p>";
  html += "<p>Der aktuelle Live-Füllstand wurde nicht verändert.</p>";
  html += "<p>Persistenz: <b>OK</b></p>";
  html += "<a class='btn' href='/'>Dashboard</a> ";
  html += "<a class='btn' href='/update'>Diagnose</a>";
  html += "</div>";
  html += pageFooter();
  sendLocalizedHtml(200, html);
}


static bool saveGeneratedCompactHistory(const CompactHistoryDay* data,
                                        uint16_t count,
                                        uint32_t baseDay,
                                        uint32_t lastDayKey,
                                        float dayStartLiters,
                                        float lastLiters,
                                        float pendingRefill) {
  if (!historyEnsureSpiffs()) return false;
  if (count == 0 || count > HISTORY_DAYS) return false;

  Preferences m;
  uint8_t active = 0;
  uint32_t gen = 0;
  if (m.begin(HISTORY_NVS_META_NS, false)) {
    active = m.getUChar("active", 0);
    gen = m.getUInt("gen", 0);
    m.end();
  }

  const uint8_t target = active ? 0 : 1;
  const uint32_t newGen = gen + 1;
  const char* tmp = target ? HISTORY_SPIFFS_TMP_B : HISTORY_SPIFFS_TMP_A;
  const char* final = target ? HISTORY_SPIFFS_FILE_B : HISTORY_SPIFFS_FILE_A;

  const size_t dataBytes = sizeof(CompactHistoryDay) * (size_t)HISTORY_DAYS;
  const uint32_t crc = historyCrc32(reinterpret_cast<const uint8_t*>(data), dataBytes);

  HistoryFileHeader h{};
  h.magic = HISTORY_SPIFFS_MAGIC;
  h.version = HISTORY_SPIFFS_VERSION;
  h.generation = newGen;
  h.count = count;
  h.index = 0;
  h.lastDayKey = lastDayKey;
  h.dayStartLiters = dayStartLiters;
  h.lastLiters = lastLiters;
  h.pendingRefill = pendingRefill;
  h.baseDay = baseDay;
  h.dataBytes = dataBytes;
  h.dataCrc = crc;

  Serial.printf("[HISTORY] Speichere %u Tage in SPIFFS-Bank %c...\n",
                (unsigned)count, target ? 'B' : 'A');

  // Inaktive Zielbank zuerst loeschen, damit fuer 10 Jahre bzw.
  // Import nicht drei komplette Historienkopien gleichzeitig in SPIFFS liegen.
  SPIFFS.remove(tmp);
  SPIFFS.remove(final);
  File f = SPIFFS.open(tmp, FILE_WRITE);
  if (!f) return false;

  bool ok = (f.write(reinterpret_cast<const uint8_t*>(&h), sizeof(h)) == sizeof(h));
  if (ok) {
    ok = (f.write(reinterpret_cast<const uint8_t*>(data), dataBytes) == dataBytes);
  }
  f.flush();
  f.close();

  if (!ok) {
    SPIFFS.remove(tmp);
    return false;
  }

  // Rohdaten verifizieren, ohne den aktiven RAM-Cache anzutasten.
  File v = SPIFFS.open(tmp, FILE_READ);
  if (!v) {
    SPIFFS.remove(tmp);
    return false;
  }
  HistoryFileHeader vh{};
  bool verified = (v.read(reinterpret_cast<uint8_t*>(&vh), sizeof(vh)) == sizeof(vh));
  if (verified) {
    verified = vh.magic == HISTORY_SPIFFS_MAGIC &&
               vh.version == HISTORY_SPIFFS_VERSION &&
               vh.count == count &&
               vh.dataBytes == dataBytes &&
               vh.dataCrc == crc;
  }
  v.close();

  if (!verified) {
    SPIFFS.remove(tmp);
    Serial.println("[HISTORY] Persistenz-Verifikation: FEHLER");
    return false;
  }

  SPIFFS.remove(final);
  if (!SPIFFS.rename(tmp, final)) {
    SPIFFS.remove(tmp);
    return false;
  }

  if (!m.begin(HISTORY_NVS_META_NS, false)) {
    SPIFFS.remove(final);
    return false;
  }
  const bool metaOk = (m.putUChar("active", target) == 1) &&
                      (m.putUInt("gen", newGen) == sizeof(uint32_t));
  m.end();

  if (!metaOk) {
    SPIFFS.remove(final);
    return false;
  }

  Serial.printf("[HISTORY] Persistenz-Verifikation: OK, Bank=%c, Gen=%lu\n",
                target ? 'B' : 'A', (unsigned long)newGen);
  return true;
}

void handleGenerate10YearTestHistory() {
  if (!historyEnsureSpiffs()) {
    server->send(500, "text/plain; charset=utf-8",
                 "10-Jahres-Testdaten abgebrochen: SPIFFS nicht verfuegbar.");
    Serial.println("[HISTORY] 10-Jahres-Testdaten abgebrochen: SPIFFS nicht verfuegbar");
    return;
  }

  time_t now = time(nullptr);
  if (now < 1000000000UL) {
    server->send(503, "text/plain; charset=utf-8",
                 "NTP-Zeit ist noch nicht synchronisiert.");
    return;
  }

  float capacity = calculateTankCapacityLiters();
  if (!isfinite(capacity) || capacity <= 0.0f) {
    server->send(500, "text/plain; charset=utf-8",
                 "Tankkapazitaet konnte nicht bestimmt werden.");
    return;
  }

  // Kompakter 10-Jahres-Puffer: 3650 * 8 Byte = 29.2 KB.
  // static => niemals auf dem loopTask-Stack.
  CompactHistoryDay *compact = historyWorkspace.compact;
  memset(compact, 0, sizeof(CompactHistoryDay) * HISTORY_DAYS);

  const uint16_t DAYS = HISTORY_DAYS;
  const uint16_t DAYS_PER_YEAR = 365;
  const uint8_t generatedSource = requestedTestHistorySource();
  const uint8_t YEARS = 10;
  const uint8_t REFILLS_PER_YEAR = 5;
  const uint16_t baseRefillDays[5] = {62, 126, 190, 248, 340};
  const float baseRefillPct[5] = {0.20f, 0.16f, 0.24f, 0.18f, 0.22f};
  const float yearFactor[10] = {1.00f, 0.97f, 1.03f, 0.96f, 1.01f,
                                1.04f, 0.98f, 1.02f, 0.95f, 1.00f};
  const float refillYearFactor[10] = {1.00f, 1.03f, 0.98f, 1.05f, 0.97f,
                                      1.02f, 1.06f, 0.95f, 1.04f, 0.99f};

  struct tm todayTm;
  localtime_r(&now, &todayTm);
  todayTm.tm_hour = 12;
  todayTm.tm_min = 0;
  todayTm.tm_sec = 0;
  const time_t todayNoon = mktime(&todayTm);
  const time_t startTime = todayNoon - (time_t)(DAYS - 1) * 86400;

  struct tm firstTm;
  localtime_r(&startTime, &firstTm);
  const uint32_t baseDay = (uint32_t)(firstTm.tm_year + 1900) * 10000UL +
                           (uint32_t)(firstTm.tm_mon + 1) * 100UL +
                           (uint32_t)firstTm.tm_mday;

  float previousLiters = capacity * 0.90f;
  uint32_t refillTotal = 0;

  for (uint16_t i = 0; i < DAYS; ++i) {
    const time_t dayTime = startTime + (time_t)i * 86400;
    struct tm dayTm;
    localtime_r(&dayTime, &dayTm);

    const uint8_t yearNo = min((uint8_t)(i / DAYS_PER_YEAR), (uint8_t)(YEARS - 1));
    const uint16_t dayInYear = i % DAYS_PER_YEAR;

    float seasonal = 1.0f + 0.55f * cosf((((float)dayTm.tm_yday - 15.0f) / 365.0f) * 6.2831853f);
    float dailyVariation = 1.0f +
                           0.08f * sinf((float)i * 1.731f) +
                           0.04f * cosf((float)i * 0.417f);
    float dailyConsumption = capacity * 0.0060f * seasonal * dailyVariation * yearFactor[yearNo];
    dailyConsumption = constrain(dailyConsumption, 0.001f, capacity * 0.025f);

    bool isRefill = false;
    uint8_t refillNo = 0;
    uint16_t refillDay = 0;
    for (uint8_t r = 0; r < REFILLS_PER_YEAR; ++r) {
      // Kleine Verschiebung pro Jahr, ohne das Grundmodell aufzublähen.
      const int shift = ((int)(yearNo % 5) - 2) * 3 + ((int)r - 2);
      int candidate = (int)baseRefillDays[r] + shift;
      candidate = constrain(candidate, 20, 340);
      if (dayInYear == (uint16_t)candidate) {
        isRefill = true;
        refillNo = r;
        refillDay = (uint16_t)candidate;
        break;
      }
    }
    (void)refillDay;

    float endLiters = previousLiters - dailyConsumption;
    float refillLiters = 0.0f;

    if (isRefill) {
      float amount = capacity * baseRefillPct[refillNo] * 2.20f * refillYearFactor[yearNo];
      amount *= 1.0f + ((int)(refillNo % 5) - 2) * 0.025f;
      const float beforeRefill = previousLiters;
      endLiters = min(beforeRefill + amount, capacity * 0.95f);
      refillLiters = max(0.0f, endLiters - beforeRefill);
      dailyConsumption = 0.0f;
      ++refillTotal;
    }

    endLiters = constrain(endLiters, capacity * 0.05f, capacity * 0.96f);
    if (!isfinite(endLiters)) endLiters = previousLiters;

    const uint32_t dayKey = (uint32_t)(dayTm.tm_year + 1900) * 10000UL +
                            (uint32_t)(dayTm.tm_mon + 1) * 100UL +
                            (uint32_t)dayTm.tm_mday;
    const int32_t offset = historyDayOffset(baseDay, dayKey);

    compact[i].dayOffset = (uint16_t)constrain(offset, 0, 65535);
    compact[i].levelLiters = (uint16_t)constrain((int)lroundf(max(0.0f, endLiters)), 0, 65535);
    compact[i].consumptionLiters = (uint16_t)constrain((int)lroundf(max(0.0f, dailyConsumption)), 0, 65535);
    compact[i].refillLiters = (uint16_t)constrain((int)lroundf(max(0.0f, refillLiters)), 0, 65535);
    compact[i].source = generatedSource;

    previousLiters = endLiters;
  }

  const uint32_t lastDayKey = baseDay + 0; // overwritten below with actual last day
  struct tm lastTm;
  localtime_r(&todayNoon, &lastTm);
  const uint32_t actualLastDayKey = (uint32_t)(lastTm.tm_year + 1900) * 10000UL +
                                    (uint32_t)(lastTm.tm_mon + 1) * 100UL +
                                    (uint32_t)lastTm.tm_mday;
  (void)lastDayKey;

  if (!saveGeneratedCompactHistory(compact, DAYS, baseDay, actualLastDayKey,
                                   capacity * 0.90f, previousLiters, 0.0f)) {
    Serial.println("[HISTORY] 10-Jahres-Testdaten abgebrochen: Persistenz fehlgeschlagen");
    server->send(500, "text/plain; charset=utf-8",
                 "10-Jahres-Testdaten konnten nicht persistent gespeichert werden.");
    return;
  }

  // Nach erfolgreichem Commit die letzten 365 Tage in den RAM-Cache laden.
  HistoryFileHeader loaded{};
  const char* finalPath = [&]() -> const char* {
    Preferences m;
    uint8_t active = 0;
    if (m.begin(HISTORY_NVS_META_NS, true)) {
      active = m.getUChar("active", 0);
      m.end();
    }
    return active ? HISTORY_SPIFFS_FILE_B : HISTORY_SPIFFS_FILE_A;
  }();

  if (!historyReadFile(finalPath, &loaded)) {
    Serial.println("[HISTORY] 10-Jahres-Testdaten gespeichert, RAM-Cache konnte nicht geladen werden");
  } else {
    historyStoreDirty = false;
  }

  Serial.printf("[HISTORY] 10-Jahres-Testdaten erzeugt: %u Tage, Nachfuellungen=%lu, Tank=%.0f L\n",
                DAYS, (unsigned long)refillTotal, capacity);
  Serial.println("[HISTORY] Saisonprofil: Winter hoch / Sommer niedrig / Uebergangszeiten mittel");
  Serial.println("[HISTORY] Kleine Variation je Jahr, Nachfuellungen +100%%");
  Serial.println("[HISTORY] Persistenz-Verifikation: OK");

  String html = pageHeader("10 Jahre Testdaten erzeugt");
  html += "<div class='card'>";
  html += "<h1>10 Jahre Testdaten erzeugt</h1>";
  html += "<p>3650 Tageswerte wurden in SPIFFS gespeichert.</p>";
  html += "<p>Testdaten skaliert auf eingestellte Tankgroesse: <b>"; html += String(capacity, 0); html += " L</b></p>";
  html += "<p>Nachfuellungen: <b>"; html += String(refillTotal); html += "</b></p>";
  html += "<p>Jedes Jahr hat kleine Variationen. Nachfuellmengen sind verdoppelt.</p>";
  html += "<p>Der RAM-Cache enthaelt weiterhin nur die letzten 365 Tage.</p>";
  html += "<p>Persistenz: <b>OK</b></p>";
  html += "<a class='btn' href='/'>Dashboard</a> ";
  html += "<a class='btn' href='/history'>Historie</a>";
  html += "</div>";
  html += pageFooter();
  sendLocalizedHtml(200, html);
}


void handleUpdatePage() {

  String html =
    pageHeader("OTA Update");

  html += "<div class='card'>";
  html += "<h1>Firmware Update</h1>";

  html += "<p>Firmware-Datei auswählen:</p>";

  html += "<form method='POST' ";
  html += "action='/update' ";
  html += "enctype='multipart/form-data'>";

  html +=
    "<input type='file' name='update' "
    "accept='.bin'>";

  html += "<button type='submit'>Update starten</button>";

  html += "</form>";

  html += "<hr style='margin:20px 0'>";
  html += "<h2>Historie testen</h2>";
  html += "<p>Saisonale Testdaten mit kleinen Jahresvariationen und 4–6 Nachfuellungen pro Jahr.</p>";
  html += "<div style='display:flex;gap:10px;flex-wrap:wrap'>";
  html += "<form method='POST' action='/generate-test-history' style='margin:0'>";
  html += "<input type='hidden' name='years' value='1'>";
  html += "<button type='submit'>📊 1 Jahr Testdaten</button>";
  html += "</form>";
  html += "<form method='POST' action='/generate-test-history-10y' style='margin:0'>";
  html += "<button type='submit'>📊 10 Jahre Testdaten</button>";
  html += "</form>";
  html += "</div>";
  html += "<p style='margin-top:10px;opacity:.8'>10 Jahre werden kompakt in SPIFFS gespeichert; im RAM bleiben nur 365 Tage.</p>";
  html += "<p><a class='btn' href='/history/import'>📥 Historische CSV importieren</a></p>";
  html += "<p><a class='btn' href='/history'>📈 Gespeicherte Historie anzeigen</a></p>";
  html += "<div style='display:flex;gap:10px;flex-wrap:wrap'>";
  html += "<form method='POST' action='/clear-consumption-data' style='margin:0' onsubmit='return confirm(\'Wirklich alle Verbrauchsdaten und die Historie löschen?\')'>";
  html += "<button type='submit' style='background:#b00020'>🗑 Verbrauchsdaten löschen</button>";
  html += "</form>";
  html += "</div>";

  html += "</div>";

  html += pageFooter();

  server->send(
    200,
    "text/html; charset=utf-8",
    html
  );
}

// ============================================================================
// WEB OTA UPLOAD
// ============================================================================

void handleUpdateUpload() {

  HTTPUpload &upload =
    server->upload();

  if (upload.status ==
      UPLOAD_FILE_START) {

    updateRunning = true;
    otaLastWebUpdateOK = false;
    otaLastWebUpdateBytes = 0;

    Serial.print(
      "[UPDATE] Start: "
    );

    Serial.println(
      upload.filename
    );

    if (!Update.begin(
          UPDATE_SIZE_UNKNOWN)) {

      Update.printError(
        Serial
      );
    }

  } else if (upload.status ==
             UPLOAD_FILE_WRITE) {

    if (!Update.hasError() &&
        Update.write(
          upload.buf,
          upload.currentSize) !=
        upload.currentSize) {

      Update.printError(
        Serial
      );
    }

  } else if (upload.status ==
             UPLOAD_FILE_END) {

    if (!Update.hasError() && Update.end(true)) {

      otaLastWebUpdateOK = true;
      otaLastWebUpdateBytes = upload.totalSize;

      Serial.print(
        "[UPDATE] Größe: "
      );

      Serial.println(
        upload.totalSize
      );

    } else {

      Update.printError(
        Serial
      );
    }

    updateRunning = false;

  } else if (upload.status ==
             UPLOAD_FILE_ABORTED) {

    Update.abort();

    otaLastWebUpdateOK = false;
    updateRunning = false;

    Serial.println(
      "[UPDATE] Abgebrochen"
    );
  }

  yield();
}

void handleUpdateFinished() {

  String html =
    pageHeader("Update");

  html += "<div class='card'>";

  if (Update.hasError() || !otaLastWebUpdateOK) {

    html += "<h1>Update fehlgeschlagen</h1>";
    html += "<p>Bitte serielle Diagnose prüfen.</p>";

  } else {

    html += "<h1>Update erfolgreich</h1>";
    html += "<p>Firmware geschrieben: <b>";
    html += String(otaLastWebUpdateBytes);
    html += " Byte</b></p>";
    html += "<p>ESP32 wird neu gestartet.</p>";
  }

  html += "</div>";

  html += pageFooter();

  server->send(
    200,
    "text/html; charset=utf-8",
    html
  );

  if (!Update.hasError() && otaLastWebUpdateOK) {

    delay(1000);

    ESP.restart();
  }
}

// ============================================================================
// WEB NOT FOUND / CAPTIVE PORTAL
// ============================================================================

void handleNotFound() {

  if (apMode) {

    server->sendHeader(
      "Location",
      String("http://") +
      WiFi.softAPIP().toString(),
      true
    );

    server->send(
      302,
      "text/plain",
      ""
    );

    return;
  }

  server->send(
    404,
    "text/plain",
    "404 Not Found"
  );
}

// ============================================================================
// WEB SERVER SETUP
// ============================================================================

void setupWebServer() {

  server->on(
    "/",
    HTTP_GET,
    handleRoot
  );

  server->on(
    "/settings",
    HTTP_GET,
    handleSettings
  );

  server->on(
    "/config",
    HTTP_GET,
    handleConfig
  );

  server->on(
    "/config/save",
    HTTP_POST,
    handleConfigSave
  );

  server->on(
    "/status",
    HTTP_GET,
    handleStatus
  );

  server->on(
    "/api/status",
    HTTP_GET,
    handleStatusJson
  );

  server->on(
    "/api/ping",
    HTTP_GET,
    handleApiPing
  );

  server->on(
    "/calibration",
    HTTP_GET,
    handleCalibration
  );

  server->on(
    "/calibration/apply",
    HTTP_POST,
    handleCalibrationApply
  );

  server->on(
    "/systemstatus",
    HTTP_GET,
    handleSystemStatusPage
  );

  server->on(
    "/history",
    HTTP_GET,
    handleHistoryPage
  );

  server->on(
    "/api/history",
    HTTP_GET,
    handleHistory
  );

  server->on(
    "/api/recent-refills",
    HTTP_GET,
    handleRecentRefills
  );

  server->on(
    "/history.csv",
    HTTP_GET,
    handleHistoryCsv
  );

  server->on(
    "/history/import",
    HTTP_GET,
    [](){ handleHistoryImportPage(); }
  );

  server->on(
    "/history/import",
    HTTP_POST,
    handleHistoryImport,
    handleHistoryImportUpload
  );

  server->on(
    "/consumption",
    HTTP_GET,
    handleConsumption
  );

  server->on(
    "/factory-reset",
    HTTP_GET,
    handleFactoryReset
  );

  server->on(
    "/reboot",
    HTTP_GET,
    handleReboot
  );

  server->on(
    "/generate-test-history",
    HTTP_POST,
    handleGenerateTestHistory
  );

  server->on(
    "/generate-test-history-10y",
    HTTP_POST,
    handleGenerate10YearTestHistory
  );

  server->on(
    "/clear-consumption-data",
    HTTP_POST,
    handleClearConsumptionData
  );

  server->on(
    "/update",
    HTTP_GET,
    handleUpdatePage
  );

  server->on(
    "/update",
    HTTP_POST,
    handleUpdateFinished,
    handleUpdateUpload
  );

  server->onNotFound(
    handleNotFound
  );

  webServerConfigured = true;

  Serial.println("[WEB] Routen registriert");
  Serial.println("[WEB] /");
  Serial.println("[WEB] /settings");
  Serial.println("[WEB] /config");
  Serial.println("[WEB] /config/save");
  Serial.println("[WEB] /status");
  Serial.println("[WEB] /api/status");
  Serial.println("[WEB] /api/ping");
  Serial.println("[WEB] /calibration");
  Serial.println("[WEB] /calibration/apply");
  Serial.println("[WEB] /systemstatus");
  Serial.println("[WEB] /history");
  Serial.println("[WEB] /api/history");
  Serial.println("[WEB] /history.csv");
  Serial.println("[WEB] /consumption");
  Serial.println("[WEB] /factory-reset");
  Serial.println("[WEB] /reboot");
  Serial.println("[WEB] /generate-test-history");
  Serial.println("[WEB] /generate-test-history-10y");
  Serial.println("[WEB] /clear-consumption-data");
  Serial.println("[WEB] /update");
    Serial.print("[WEB] API-Test: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/api/ping");
}

// ============================================================================
// WEB TASK
// ============================================================================

void webTask() {
  if (!webServerStarted || server == nullptr) {
    return;
  }

  server->handleClient();
}

// ============================================================================
// SERIAL DEBUG
// ============================================================================

static bool debugEnabled(uint8_t requiredLevel) {
  return cfg.debugLevel >= requiredLevel;
}

static void serialPrintBoolStatus(const __FlashStringHelper* label, bool status) {
  Serial.print(label);
  Serial.println(status ? F("OK") : F("OFF"));
}

static void serialPrintSensorStatus(const __FlashStringHelper* label, bool status) {
  Serial.print(label);
  Serial.println(status ? F("OK") : F("ERROR"));
}

static void serialPrintRuntimeLine(const __FlashStringHelper* label, const String &value) {
  Serial.print(label);
  Serial.println(value);
}

static void serialPrintRuntimeLine(const __FlashStringHelper* label, float value, uint8_t decimals, const char* unit) {
  Serial.print(label);
  Serial.print(value, decimals);
  Serial.println(unit);
}

void serialDebugTask() {

  if (!debugEnabled(DEBUG_LEVEL_NORMAL)) {
    return;
  }

  uint32_t now =
    millis();

  if (now - lastDebug <
      DEBUG_INTERVAL) {
    return;
  }

  lastDebug = now;

  Serial.println();
  Serial.println(F("========== STATUS =========="));

  Serial.print(F("FW: "));
  Serial.println(FW_VERSION);

  Serial.print(F("State: "));
  Serial.println(stateToString());

  serialPrintBoolStatus(F("WiFi: "), WiFi.status() == WL_CONNECTED);

  if (WiFi.status() == WL_CONNECTED) {
    serialPrintRuntimeLine(F("IP: "), String(WiFi.localIP().toString()));
    Serial.print(F("RSSI: "));
    Serial.println(WiFi.RSSI());
  }

  serialPrintBoolStatus(F("MQTT: "), mqttClient.connected());
  serialPrintSensorStatus(F("VL: "), vlOK);

  Serial.print(F("Sensorfehler: "));
  Serial.println(sensorErrorCount);

  Serial.print(F("Sensor-Reinit: "));
  Serial.println(sensorReinitCount);

  serialPrintSensorStatus(F("BME: "), bmeOK);
  serialPrintSensorStatus(F("OLED: "), oledOK);

  Serial.print(F("Messungen: "));
  Serial.println(measurementCounter);

  serialPrintRuntimeLine(F("Füllstand: "), tankPercent, 1, " %");
  serialPrintRuntimeLine(F("Liter: "), tankLiters, 1, " L");

  Serial.println(F("============================"));
}



#endif // FUELLSTANDSMESSER3_UNITY_BUILD
