<?php

function _checkbook_alerts_form(){
  echo theme('checkbook_alerts_default_theme',array());
}

function _checkbook_alerts_advanced_search_form(){
    echo theme('checkbook_alerts_advanced_search_theme',array());
}

function _checkbook_alerts_getQuery(){
$sql=
  "SELECT checkbook_alerts_sysid,".
    "label,recipient,".
    "recipient_type,".
    "ref_url,".
    "user_url,".
    "active,".
    "number_of_results,".
    "minimum_results,".
    "minimum_days,".
    "date_end,".
    "date_last_new_results ".
  "FROM checkbook_alerts ";
  return $sql;
}

function _checkbook_alerts_transactions(){

  //Give default or option to pass this
  if(!isset($_GET['alert_theme_file'])){
      $_GET['alert_theme_file']='checkbook_alerts_subscribe_theme';
  }
  if(!isset($_GET['alert_minimum_results'])){
    $_GET['alert_minimum_results']=1;
  }
  if(!isset($_GET['alert_minimum_days'])){
    $_GET['alert_minimum_days']=1;
  }

  $_GET['q']=urldecode(drupal_get_path_alias($_GET['refURL']));

  $alerts=db_query(
    _checkbook_alerts_getQuery().
    "WHERE recipient=:recipient AND ".
      "recipient_type='EMAIL' AND ".
      "UPPER(ref_url)=UPPER(:ref_url)",
        array(
          ":recipient"=>$_GET['alert_email'],
          ":ref_url"=>$_GET['refURL']));

  $alertEndDate=strtotime($_GET['alert_end']);
  if(!$alertEndDate){
    $alertEndDate = strtotime(date("Y-m-d", mktime()) . " + 365 day");
  }
  $alertEndDate=date("Y-m-d H:i:s",$alertEndDate);
  
  $alert=array(
    'recipient'=>$_GET['alert_email'],
    'label'=>$_GET['alert_label'],
    'recipient_type'=>'EMAIL',
    'ref_url'=>$_GET['refURL'],
    'user_url'=>$_GET['userURL'],
    'number_of_results'=>_checkbook_alerts_get_data_count(),
    'minimum_results'=>$_GET['alert_minimum_results'],
    'minimum_days'=>$_GET['alert_minimum_days'],
    'date_end'=>$alertEndDate,
    'date_last_new_results'=>date("Y-m-d H:i:s")
  );

  if(($a=$alerts->fetchObject())!=null){
    $alert['checkbook_alerts_sysid']=$a->checkbook_alerts_sysid;
    $alert['active']=$a->active;
    drupal_write_record('checkbook_alerts',$alert,array("checkbook_alerts_sysid"));
  }else{
    drupal_write_record('checkbook_alerts',$alert);
  }
  $alertFrequency=_checkbook_alerts_alertFrequency($_GET['alert_minimum_days']);
  
  $subData=array(
    "alertFrequency"=>$alertFrequency,
    "label"=>$_GET['alert_label'],
     "alert2"=>$alert,
  );

  $module="checkbook_alerts";
  $key=md5(date("Y-m-d H:i:s"));
  $msg=theme('checkbook_alerts_activate_theme',$subData);

  $message=drupal_mail($module,$key,$_GET['alert_email'],language_default(),array(),"",false);

  $message['subject']="Checkbook NYC Alert Activation";
  $message['body'][] = $msg;

  $system=drupal_mail_system($module,$key);
  $message=$system->format($message);

  $system->mail($message);

  $res=array(
    "success"=>true,
    "html"=>theme($_GET['alert_theme_file'],$subData)
  );
  echo json_encode($res);
}

function _checkbook_alerts_alertFrequency($days){
  switch($days){
  case 1:
    $alertFrequency="Daily";
    break;
  case 7:
    $alertFrequency="Weekly";
    break;
  case 30:
    $alertFrequency="Monthly";
    break;
  case 90:
    $alertFrequency="Quarterly";
    break;
  default:
    $alertFrequency="{$_GET['alert_minimum_days']} days";
  }
  return $alertFrequency;
}

function _checkbook_alerts_process_email($alerts){
  //$module="checkbook_alerts";
  //$key=md5(date("Y-m-d H:i:s"));
  $msg=theme('checkbook_alerts_email_theme',array(
    'alerts'=>$alerts
  ));

  //$message=drupal_mail($module,$key,$alerts[0]->recipient,language_default(),array(),"",false);

  //$message['subject']="Checkbook NYC Alert";
  //$message['body'][] = $msg;
  mail($alerts[0]->recipient,"Checkbook NYC Alert",$msg,"From: checkbook@reisys.com\r\nContent-Type: text/html;");
  //$system=drupal_mail_system($module,$key);
  //$message=$system->format($message);

  //$message['result']=$system->mail($message);
}

function _checkbook_alerts_process_twitter($alert){
  global $conf;

  if(!isset($conf['check_book']['alerts']['twitter'][$alert->recipient])){
    return;
  }

  $settings = $conf['check_book']['alerts']['twitter'][$alert->recipient];

  $message=str_replace(":count",$alert->new_count,$alert->label);
  $url = 'https://api.twitter.com/1.1/statuses/update.json';

  $postfields = array(
    'status'=>$message,
    'wrap_links'=>'true'
  );
  $requestMethod = 'POST';
  $twitter = new TwitterAPIExchange($settings);
  echo $twitter->buildOauth($url,$requestMethod)
               ->setPostfields($postfields)
               ->performRequest();
}

function _checkbook_alerts_unsubscribe(){
  $alert=arg(2);

  db_query('DELETE FROM checkbook_alerts WHERE CONCAT(checkbook_alerts_sysid,MD5(CONCAT(checkbook_alerts_sysid,label,recipient)))=:alert OR MD5(recipient)=:alert',array(':alert'=>$alert));

  return theme('checkbook_alerts_unsubscribe_theme');
}

function _checkbook_alerts_activate(){
  $alert=arg(2);

    db_update('checkbook_alerts')
        ->fields(array('active' => 'Y'))
        ->where('CONCAT(checkbook_alerts_sysid,MD5(CONCAT(checkbook_alerts_sysid,label,recipient))) = :alert', array(':alert' => $alert))
        ->execute();

 // db_query("UPDATE checkbook_alerts SET active='Y' WHERE CONCAT(checkbook_alerts_sysid,MD5(CONCAT(checkbook_alerts_sysid,label,recipient)))=:alert",array(':alert'=>$alert));

  return theme('checkbook_alerts_activated_theme',array("arg2" => $alert));
}

function _checkbook_alerts_twitter_callback(){

}

//////////////////////////////////////////////////////////

function _checkbook_alerts_get_data_count(){
  $node = _checkbook_alerts_get_node_config();

  $node->widgetConfig->getTotalDataCount = true;
  $node->widgetConfig->getData = false;

  widget_data($node);



  return $node->totalDataCount;
}

function _checkbook_alerts_get_node_config(){
  global $conf;

  $_GET['q']=urldecode(check_plain($_GET['refURL']));

  $nodeId = _getRequestParamValue('node');

  $node = node_load($nodeId);

  widget_config($node);

  widget_prepare($node);

  widget_invoke($node, 'widget_prepare');

  return $node;
}
