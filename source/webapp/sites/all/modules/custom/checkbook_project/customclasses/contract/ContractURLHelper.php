<?php
/**
* This file is part of the Checkbook NYC financial transparency software.
* 
* Copyright (C) 2012, 2013 New York City
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
* 
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


class ContractURLHelper{

    static $landingPageParams = array("status"=>"status","vendor"=>"vendor","agency"=>"agency","awdmethod"=>"awdmethod","cindustry"=>"cindustry","csize"=>"csize");
    static $transactionPageParams = array("status"=>"status","vendor"=>"cvendor","agency"=>"cagency","awdmethod"=>"awdmethod","cindustry"=>"cindustry","csize"=>"csize");

    static function prepareExpenseContractLink($row, $node) {

        $link = NULL;
        $docType = $row['document_code@checkbook:ref_document_code'];
        $agrParamName = in_array($docType, array('MMA1','MA1')) ? 'magid' : 'agid';

        if( RequestUtil::isExpandBottomContainer() ){
          $link = '<a href=/panel_html/contract_transactions/contract_details/' .$agrParamName . '/' . $row['original_agreement_id'] .  '/doctype/' . $docType .  _checkbook_append_url_params() . ' class=bottomContainerReload>'. $row['contract_number'] . '</a>';
        } else {
          $link = '<a href=/contracts_landing'
              . _checkbook_project_get_url_param_string('contstatus','status')
              . _checkbook_append_url_params()
              .  (
                    isset($row['type_of_year@checkbook:contracts_coa_aggregates']) ?
                            ( $row['type_of_year@checkbook:contracts_coa_aggregates'] == 'B' ? ('/yeartype/B/year/'. $row['fiscal_year_id@checkbook:contracts_coa_aggregates']) : ('/yeartype/C/calyear/'.$row['fiscal_year_id@checkbook:contracts_coa_aggregates']) )
                            : (_checkbook_project_get_year_url_param_string())
                 )
              . ((_checkbook_check_isEDCPage()) ? '/agency/' . $row['agency_id'] :'')
              . '?expandBottomContURL=/panel_html/contract_transactions/contract_details/' .$agrParamName . '/' . $row['original_agreement_id'] .  '/doctype/' . $docType . _checkbook_append_url_params()
              .  ' >'. $row['contract_number'] . '</a>';
        }

        return $link;
    }

    static function prepareRevenueContractLink($row, $node) {

        $link = NULL;
        $docType = $row['document_code@checkbook:ref_document_code'];
        $agrParamName = 'magid';//in_array($docType, array('MMA1','MA1')) ? 'magid' : 'agid';

        if( RequestUtil::isExpandBottomContainer() ){
          $link = '<a href=/panel_html/contract_transactions/' .$agrParamName . '/' . $row['original_agreement_id'] .  '/doctype/' . $docType .  ' class=bottomContainerReload>'. $row['contract_number'] . '</a>';
        } else {
          $link = '<a href=/contracts_revenue_landing'
              . _checkbook_project_get_url_param_string('contstatus','status')
              .  (
                  isset($row['type_of_year@checkbook:contracts_coa_aggregates']) ?
                      ( $row['type_of_year@checkbook:contracts_coa_aggregates'] == 'B' ? ('/yeartype/B/year/'. $row['fiscal_year_id@checkbook:contracts_coa_aggregates']) : ('/yeartype/C/calyear/'.$row['fiscal_year_id@checkbook:contracts_coa_aggregates']) )
                      : (_checkbook_project_get_year_url_param_string())
                  )
              . '?expandBottomContURL=/panel_html/contract_transactions/' .$agrParamName . '/' . $row['original_agreement_id'] .  '/doctype/' . $docType
              .  ' >'. $row['contract_number'] . '</a>';
        }

        return $link;
    }

    static function preparePendingContractLink($row, $node) {

        $agreementId = $row['original_agreement_id'];
        if(!isset($agreementId)){//No link if mag is not present
          return '<a class="bottomContainerReload" href = "/minipanels/pending_contract_transactions/contract/'.$row['fms_contract_number'].'/version/'.$row['document_version'].'">'.$row['contract_number'].'</a>';
        }

        $link = NULL;
        $docType = $row['document_code@checkbook:ref_document_code'];
        $agrParamName = in_array($docType, array('MMA1','MA1','RCT1')) ? 'magid' : 'agid';

        if( RequestUtil::isExpandBottomContainer() ){
          $link = '<a href=/panel_html/contract_transactions/' .$agrParamName . '/' . $row['original_agreement_id'] .  '/doctype/' . $docType .  ' class=bottomContainerReload>'. $row['contract_number'] . '</a>';
        } else {
          $link = '<a href=/' . ($docType == 'RCT1' ? 'contracts_pending_rev_landing': 'contracts_pending_exp_landing') .'/'
              . _checkbook_project_get_url_param_string('contstatus','status')
              .  (
                  isset($row['type_of_year@checkbook:contracts_coa_aggregates']) ?
                      ( $row['type_of_year@checkbook:contracts_coa_aggregates'] == 'B' ? ('/yeartype/B/year/'. $row['fiscal_year_id@checkbook:contracts_coa_aggregates']) : ('/yeartype/C/calyear/'.$row['fiscal_year_id@checkbook:contracts_coa_aggregates']) )
                      : (_checkbook_project_get_year_url_param_string())
                  )
              . '?expandBottomContURL=/panel_html/contract_transactions/' .$agrParamName . '/' . $row['original_agreement_id'] .  '/doctype/' . $docType
              .  ' >'. $row['contract_number'] . '</a>';
        }

        return $link;
    }

    static function prepareSpendingContractLink($row, $node) {
        if($row['spending_category_name'] == 'Payroll' ||  $row['spending_category_name'] == 'Others') {
          return 'N/A';
        }

        if(empty($row[agreement_id])){
            return $row[reference_document_number];
        }

        $link = NULL;
        $docType = $row['reference_document_code'];
    
        if( RequestUtil::isExpandBottomContainer() ){
            $link = '<a href=/panel_html/contract_transactions/contract_details/agid/' . $row['agreement_id'] .  '/doctype/' . $docType .  _checkbook_append_url_params() . ' class=bottomContainerReload>'. $row['reference_document_number'] . '</a>';
        }else if( RequestUtil::isNewWindow() ){
            $link = '<span href=/contracts_landing/status/A'
                . _checkbook_project_get_year_url_param_string()
                . _checkbook_append_url_params()
                . '?expandBottomContURL=/panel_html/contract_transactions/contract_details/agid/' . $row['agreement_id'] .  '/doctype/' . $docType . _checkbook_append_url_params()
                .  ' class=loadParentWindow>'. $row['reference_document_number'] . '</span>';
        }else {
            $link = "<a class='new_window' href='/contract_details" . _checkbook_append_url_params() . _checkbook_project_get_contract_url($row[reference_document_number], $row[agreement_id])  ."/newwindow'>"  . $row[reference_document_number] . "</a>";
        }

        return $link;
    }

    static function prepareSpendingContractTransactionsLink($row, $node) {

        $link = NULL;
        $docType = $row['document_code@checkbook:ref_document_code'];

        if( RequestUtil::isExpandBottomContainer() ){
            $link = '<a href=/panel_html/contract_transactions/contract_details/agid/' . $row['disb_agreement_id'] .  '/doctype/' . $docType . _checkbook_append_url_params() . ' class=bottomContainerReload>'. $row['disb_contract_number'] . '</a>';
        }else if( RequestUtil::isNewWindow() ){
            $link = '<span href=/contracts_landing/status/A'
                . _checkbook_project_get_year_url_param_string()
                . '?expandBottomContURL=/panel_html/contract_transactions/contract_details/agid/' . $row['disb_agreement_id'] .  '/doctype/' . $docType . _checkbook_append_url_params()
                .  ' class=loadParentWindow>'. $row['disb_contract_number'] . '</span>';
        }else {
            $link = '<a href=/contracts_landing/status/A'
                . _checkbook_project_get_year_url_param_string()
                . '?expandBottomContURL=/panel_html/contract_transactions/contract_details/agid/' . $row['disb_agreement_id'] .  '/doctype/' . $docType . _checkbook_append_url_params()
                .  ' >'. $row['disb_contract_number'] . '</a>';
        }

        return $link;
    }
    
    public function prepareActRegContractsSliderFilter($page, $status){
        
        $pathParams = explode('/',drupal_get_path_alias($_GET['q']));
        $url = $page;
        if(strlen($status) > 0){
            $url .= "/status/".$status;
        }
        $url .= _checkbook_append_url_params();
        if( preg_match("/^contracts_pending/", drupal_get_path_alias($_GET['q'])) ){
          $allowedFilters =  array("agency","vendor","awrdmthd","csize","cindustry","agid");
          $url .= "/yeartype/B/year/"._getCurrentYearID();
        }
        else{
          $allowedFilters =  array("year","calyear","agency","yeartype","awdmethod","vendor","csize","cindustry","agid");
        }
        for($i=1;$i < count($pathParams);$i++){

            if(in_array($pathParams[$i] ,$allowedFilters) ){
                 $url .= '/'.$pathParams[$i].'/'.$pathParams[($i+1)];
            }
            $i++;
        }
        return $url;

    }

    public function preparePendingContractsSliderFilter($page){

        $pathParams = explode('/',drupal_get_path_alias($_GET['q']));
        $url = $page;
        if( preg_match("/^contracts_pending/", drupal_get_path_alias($_GET['q'])) ){
          $allowedFilters =  array("year","calyear","agency","yeartype","awrdmthd","vendor","csize","cindustry");
               
        }
        else{
          $allowedFilters =  array("year","calyear","agency","yeartype","awdmethod","vendor","csize","cindustry");
        }  
        for($i=1;$i < count($pathParams);$i++){
        
          if(in_array($pathParams[$i] ,$allowedFilters) ){
            $url .= '/'.$pathParams[$i].'/'.$pathParams[($i+1)];
          }
          $i++;
        }     
        return $url;

    }


    /**
     * returns the year type and year values string to be appended to the URL for spending trans link.
     * @return string
     */
    function _checkbook_project_spending_get_year_url_param_string(){
        $urlPath = drupal_get_path_alias($_GET['q']);
        $pathParams = explode('/', $urlPath);

        $yeartypeIndex = array_search("yeartype",$pathParams);
        $yearIndex = array_search("year",$pathParams);
        $calyearIndex = array_search("calyear",$pathParams);

        if($yeartypeIndex){
            $yeartypeValue = $pathParams[($yeartypeIndex+1)];
            if($yeartypeValue == 'B'){
                return _checkbook_append_url_params()."/yeartype/B/year/" . $pathParams[($yearIndex+1)] . "/syear/" . $pathParams[($yearIndex+1)];
            }else{
                $year = ($calyearIndex)?$pathParams[($calyearIndex+1)]:$pathParams[($yearIndex+1)];
                return _checkbook_append_url_params()."/yeartype/C/calyear/" . $year . "/scalyear/" . $year;
            }

        }
    }

    function _prepare_oge_contracts_spending_url($row, $node){
       $agencies = _checkbook_project_querydataset('checkbook_oge:agency',array('agency_id','agency_name'),array('agency_id'=>$row['agency_id'], 'is_oge_agency'=>'Y'));
       $oge_agency_name = $agencies[0]['agency_name'];

       $vendors = _checkbook_project_querydataset('checkbook_oge:vendor',array('vendor_id','legal_name'),array('vendor_id'=>$row['vendor_id']));
       $oge_vendor_name = $vendors[0]['legal_name'];

       $vendor_url = '';
       if(strtolower($oge_agency_name) != strtolower($oge_vendor_name)){
           $vendor_url = '/vendor/' . $row['vendor_id'];
       }

       $year_url = '';
       if((!(_getRequestParamValue('year') || _getRequestParamValue('calyear')))){
           $year_url = '/yeartype/B/year/' . _getFiscalYearID() . '/syear/'. _getFiscalYearID();
       }else{
           $year_url = $row['type_of_year'] == 'B' ? ('/year/'. $row['fiscal_year_id'].'/syear/'. $row['fiscal_year_id']) : ('/calyear/'.$row['fiscal_year_id']. '/scalyear/'.$row['fiscal_year_id']);
       }

       $url = "<a href='/spending/transactions"
             .  ($row['master_agreement_yn'] == 'Y' ? '/magid/' : '/agid/') . $row['original_agreement_id']
             .  ($row['master_agreement_yn'] == 'Y' ? $vendor_url : '/vendor/' . $row['vendor_id'])
             .  ($row['master_agreement_yn'] == 'Y' ? '' : ('/scomline/'.$row['fms_commodity_line']))
             .  $year_url
             . _checkbook_project_get_url_param_string('agency')
             . _checkbook_append_url_params()
             . '/dtsmnid/' . $node->nid
             .  "/newwindow' class='new_window'>" . custom_number_formatter_basic_format($row['spending_amount_disb']) . '</a>';
        return $url;
    }

}
