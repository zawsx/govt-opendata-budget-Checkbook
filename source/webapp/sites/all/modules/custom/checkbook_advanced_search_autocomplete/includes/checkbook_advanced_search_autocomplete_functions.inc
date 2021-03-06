<?php

// Spending autocomplete menu callback functions
/**
 * Construct query string and get results from Solr for spending domain.
 * @param string $facet
 *   Facet name for which auto suggestions needed
 * @param string $query
 *   Partially constructed query
 * @param string/null $year
 *   Year value
 * @param string/null $agency
 *   Selected agency id
 * @param string/null $expcategory
 *   Selected Expense Category Name
 * @param string/null $dept
 *   Selected Department Name
 * @param string/null $exptype
 *   Selected Spending Category Id
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param $data_source
 *   Data source to get data (i.e checkbook, checkbook_oge)
 *
 * @return array
 *   Results from Solr
 ***/
function _get_spending_advanced_search_autocomplete_results($facet, $query, $year = null, $agency = null, $expcategory = null, $dept = null, $exptype = null, $mwbe_category = null, $data_source = 'checkbook'){
    if ($year) {
        $query .= _checkbook_autocomplete_get_start_end_dates($year, 'spending');
    }
    if ($dept) {
        $query .= '&fq=department_name:'._checkbook_autocomplete_escapeSolrValue(str_replace('__','/',$dept));
    }
    if ($agency) {
        $query .= '&fq=agency_id:' . $agency;
    }
    if ($expcategory) {
        $query .= '&fq=expenditure_object_name:' . '"'._checkbook_autocomplete_escapeSolrValue(trim($expcategory)).'"';
    }
    if($exptype){
        $query .= '&fq=spending_category_id:'.$exptype;
    }
    if($mwbe_category){
        $query .= '&fq=minority_type_id:'._checkbook_advanced_search_autocomplete_get_mwbe_category($mwbe_category);
    }
    $matches = _checkbook_autocomplete_solr_results($query, $facet, $data_source);
    return $matches;
}

/**
 * Returns the options for Expense Category Dropdown based on the selected agency/department/year/spending category/mwbe category values.
 *
 * @param string $agency
 *   Selected agency id
 * @param string $dept
 *   Selected Department Name
 * @param string $spending_cat
 *   Selected Spending Category Id
 * @param string $year
 *   Year value
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param $data_source
 *   Data source to get data (i.e checkbook, checkbook_oge)
 *
 * @return array
 *   Results from Query
 ***/
function _get_spending_expense_cat_options($agency, $dept, $spending_cat, $year, $mwbe_category, $data_source = 'checkbook') {

    if (strpos($year, '~')) {
        if ($year == 'fy~all') {
            $year = "";
        }
        $year_type = substr($year, 0, 2);
        $year_id = substr($year, 3, strlen($year));
        $year_value = _getYearValueFromID($year_id);
    }
    if ($year && $year_type == 'fy') {
        $yearstring = " AND fiscal_year = " . $year_value;
    }
    elseif ($year && $year_type == 'cy') {
        $yearstring = " AND calendar_fiscal_year = " . $year_value;
    }
    if ($agency) {
        $agencystring = " AND agency_id = " . $agency;
    }

    $scstring = ($spending_cat)?" spending_category_id = " . $spending_cat:" spending_category_id IN (1,2,3,4,5)";

    if ($dept){
        $deptstring = " AND department_name ILIKE '".trim($dept)."' ";
    }
    $query = "SELECT DISTINCT expenditure_object_name FROM disbursement_line_item_details
            WHERE ".$scstring.$yearstring.$agencystring.$deptstring."ORDER BY expenditure_object_name ASC";

    $db_name = "main";
    $results = _checkbook_project_execute_sql($query, $db_name,  $data_source);
    if(count($results) == 0){
        $output = array('label' => 'No Matches Found','value' => '');
    }else{
        foreach ($results as $result) {
            $output[$result['expenditure_object_name']] = $result['expenditure_object_name'];
        }
    }
    return $output;
}

/**
 * Returns the auto suggestions for vendor name based on the selected agency/department/year/spending category/expense category/mwbe category values.
 *
 * @param string $year
 *   Year value
 * @param string $agency
 *   Selected agency id
 * @param $expcategory
 *   Selected Expense Category
 * @param string $dept
 *   Selected Department Name
 * @param int $exptype
 *   Selected Spending Category Id
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param $data_source
 *   Data source to get data (i.e checkbook, checkbook_oge)
 *
 * @return array
 *   Results from Solr
 ***/
function _checkbook_advanced_search_autocomplete_spending_payee($year, $agency, $expcategory, $dept, $exptype = 0, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if (!empty($searchTerm)) {
        $query = 'select/?q=vendor_name_autocomplete:' . $searchTerm . '&facet=true&facet.field=vendor_name&fq=domain:spending&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_spending_advanced_search_autocomplete_results('vendor_name', $query, $year, $agency, $expcategory, $dept, $exptype, $mwbe_category);
        drupal_json_output($matches);
    }
}

/**
 * Returns the auto suggestions Contract ID based on the selected agency/department/year/spending category/expense category/mwbe category values.
 *
 * @param string $year
 *   Year value
 * @param string $agency
 *   Selected agency id
 * @param $expcategory
 *   Selected Expense Category
 * @param string $dept
 *   Selected Department Name
 * @param int $exptype
 *   Selected Spending Category Id
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 *
 * @return array
 *   Results from Solr
 ***/
function _checkbook_advanced_search_autocomplete_spending_contractno($year, $agency, $expcategory, $dept, $exptype = 0, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if (!empty($searchTerm)) {
        $doc_type1 = substr(trim(htmlspecialchars_decode($_REQUEST['term'], ENT_QUOTES)), 0, 3);
        $doc_type2 = substr(trim(htmlspecialchars_decode($_REQUEST['term'], ENT_QUOTES)), 0, 4);

        if(strtolower($doc_type1) == 'ma1' || strtolower($doc_type2) == 'mma1'){
            $matches = array(array('label'=>'No Matches Found','value'=>''));
        }else{
            $query = 'select/?q=contract_number:' . $searchTerm . '&facet=true&facet.field=contract_number&fq=domain:spending&facet.limit=10&wt=php';
            $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
            $matches = _get_spending_advanced_search_autocomplete_results('contract_number', $query, $year, $agency, $expcategory, $dept, $exptype, $mwbe_category);
        }
        drupal_json_output($matches);
    }
}

/**
 * Returns the auto suggestions for capital project based on the selected agency/department/year/spending category/expense category/mwbe category values.
 * @param string $year
 *   Year value
 * @param string $agency
 *   Selected agency id
 * @param string $dept
 *   Selected Department Name
 * @param int $exptype
 *   Selected Spending Category Id
 * @param $expcategory
 *   Selected Expense Category
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 *
 * @return array
 *   Results from Solr
 ***/
function _checkbook_advanced_search_autocomplete_spending_capitalproject($year, $agency, $expcategory, $dept, $exptype = 0, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if (!empty($searchTerm)) {
        $query = 'select/?q=reporting_code_autocomplete:' . $searchTerm . '&facet=true&facet.field=reporting_code&fq=domain:spending&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_spending_advanced_search_autocomplete_results('reporting_code', $query, $year, $agency, $expcategory, $dept, $exptype, $mwbe_category);
        drupal_json_output($matches);
    }
}

/**
 * Returns the auto suggestions for expense id based on the selected agency/department/year/spending category/expense category/mwbe category values.
 * @param string $year
 *   Year value
 * @param string $agency
 *   Selected agency id
 * @param string $dept
 *   Selected Department Name
 * @param int $exptype
 *   Selected Spending Category Id
 * @param $expcategory
 *   Selected Expense Category
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 *
 * @return array
 *   Results from Solr
 ***/
function _checkbook_advanced_search_autocomplete_spending_expenseid($year, $agency, $expcategory, $dept, $exptype = 0, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if (!empty($searchTerm)) {
        $query = 'select/?q=expense_id:' . $searchTerm . '&facet=true&facet.field=expense_id&fq=domain:spending&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_spending_advanced_search_autocomplete_results('expense_id', $query, $year, $agency, $expcategory, $dept, $exptype, $mwbe_category);
        drupal_json_output($matches);
    }
}

/**
 * Returns the options for department drop down based on the selected agency/year/spending category values.
 *
 * @param string $year
 *   Year value
 * @param string $agency
 *   Selected agency id
 * @param string $exp_type
 *   Selected Spending Category Id
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 *
 * @return array
 *   Results from Solr
 ***/
function _checkbook_advanced_search_autocomplete_spending_department($year, $agency, $exp_type, $data_source = 'checkbook') {
    if ($year) {
        $year_type = substr($year, 0, 2);
        $year_value = substr($year, 3, strlen($year));
        if ($year_type == 'fy') {
            $year_string = "AND disbursement_line_item_details.check_eft_issued_nyc_year_id = '" . $year_value . "' ";
        } elseif ($year_type == 'cy') {
            $year_string = "AND disbursement_line_item_details.calendar_fiscal_year_id = '" . $year_value . "' ";
        }
    }
    $agency_string = $agency ? "AND disbursement_line_item_details.agency_id = '" . $agency . "' " : "";
    $exp_type_string = $exp_type ? "AND ref_spending_category.spending_category_id = '" . $exp_type . "' " : "";
    $query = "SELECT DISTINCT department_name
            FROM disbursement_line_item_details, ref_spending_category
            WHERE disbursement_line_item_details.spending_category_id = ref_spending_category.spending_category_id " .
        $agency_string . $year_string . $exp_type_string . "ORDER BY department_name ASC";

    $db_name = "main";
    $results = _checkbook_project_execute_sql($query, $db_name,  $data_source);
    if(count($results) == 0){
        $options = array('label' => 'No Matches Found','value' => '');
    }else{
        foreach ($results as $result) {
            $options[$result['department_name']] = $result['department_name'];
        }
    }
    $matches = array();
    if(isset($options['label'])) {
        $matches[] = $options['label'];
    }
    else {
        foreach($options as $key => $value)
            $matches[] = htmlentities($value);
    }
    drupal_json_output($matches);
}

/**
 * Returns the options for Expense Category dropdown based on the selected agency/department/year/spending category values.
 * @param string $year
 *   Year value
 * @param string $agency
 *   Selected agency id
 * @param string $dept
 *   Selected Department Name
 * @param string $exptype
 *   Selected Spending Category Id
 * @param $dept
 *   Selected Department
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 ***/
function _checkbook_advanced_search_autocomplete_spending_expcategory($year, $agency, $dept, $exptype, $data_source = 'checkbook') {
    $options = _get_spending_expense_cat_options($agency, str_replace('__','/',$dept), $exptype, $year, $data_source);
    $matches = array();
    if(isset($options['label'])) {
        $matches[] = $options['label'];
    }
    else {
        foreach($options as $key => $value)
            $matches[] = htmlentities($value);
    }
    drupal_json_output($matches);
}


/**
 * Get autocomplete results for Entity Contract Number from Solr.
 *
 * @param string $year
 *   Year value
 * @param string $agency
 *   Selected agency id
 * @param $expcategory
 *   Selected Expense Category
 * @param string $dept
 *   Selected Department Name
 * @param int $exptype
 *   Selected Spending Category Id
 * @param $dept
 *   Selected Department
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 *
 * @return array
 *   Results from Solr
 ***/
function _checkbook_advanced_search_autocomplete_spending_entitycontractnum($year, $agency, $expcategory, $dept, $exptype = 0, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if (!empty($searchTerm)) {
        $query = 'select/?q=spending_entity_contract_number:' . $searchTerm . '&facet=true&facet.field=spending_entity_contract_number&fq=domain:spending&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_spending_advanced_search_autocomplete_results('spending_entity_contract_number', $query, $year, $agency, $expcategory, $dept, $exptype, $mwbe_category);
        drupal_json_output($matches);
    }
}

/**
 * Get autocomplete results for Entity Commodity Line from Solr.
 *
 * @param string $year
 *   Year value
 * @param string $agency
 *   Selected agency id
 * @param $expcategory
 *   Selected Expense Category
 * @param string $dept
 *   Selected Department Name
 * @param int $exptype
 *   Selected Spending Category Id
 * @param $dept
 *   Selected Department
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 *
 * @return array
 *   Results from Solr
 ***/
function _checkbook_advanced_search_autocomplete_spending_commodityline($year, $agency, $expcategory, $dept, $exptype = 0, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if(strlen($searchTerm) > 0){
        $query = 'select/?q=spending_commodity_line:'. $searchTerm .'&facet=true&facet.field=spending_commodity_line&fq=domain:spending&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_spending_advanced_search_autocomplete_results('spending_commodity_line', $query, $year, $agency, $expcategory, $dept, $exptype, $mwbe_category);
        drupal_json_output($matches);
    }
}

/**
 * Get autocomplete results for Budget Name from Solr.
 *
 * @param string $year
 *   Year value
 * @param string $agency
 *   Selected agency id
 * @param $expcategory
 *   Selected Expense Category
 * @param string $dept
 *   Selected Department Name
 * @param int $exptype
 *   Selected Spending Category Id
 * @param $dept
 *   Selected Department
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 *
 * @return array
 *   Results from Solr
 ***/
function _checkbook_advanced_search_autocomplete_spending_budgetname($year, $agency, $expcategory, $dept, $exptype = 0, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if(strlen($searchTerm) > 0){
        $query = 'select/?q=spending_budget_name_autocomplete:'. $searchTerm .'&facet=true&facet.field=spending_budget_name&fq=domain:spending&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_spending_advanced_search_autocomplete_results('spending_budget_name', $query, $year, $agency, $expcategory, $dept, $exptype, $mwbe_category, $mwbe_category);
        drupal_json_output($matches);
    }
}

//Payroll autocomplete menu callback functions
/**
 * Returns auto suggestions for employee title based on the selected agency/pay frequency/year values.
 * @param string $year
 *   Year value
 * @param string $agency
 *   Selected agency id
 * @param string $pay_frequency
 *   Selected Pay Frequency
 ***/
function _checkbook_advanced_search_autocomplete_payroll_employee_name($pay_frequency, $agency, $year){
    $searchTerm = _get_autocomplete_search_term();
    $query = 'select/?q=civil_service_title_autocomplete:'.$searchTerm.'&facet=true&facet.field=civil_service_title&fq=domain:payroll&facet.limit=10&wt=php';
    if(!empty($searchTerm)){
        if($pay_frequency){
            $query .= '&fq=pay_frequency:'.$pay_frequency;
        }
        if($agency){
            $query .= '&fq=agency_id:'.$agency;
        }
        if($year){
            $year_type = substr($year, 0, 2);
            $year_value = substr($year, 3,strlen($year));
            if($year_type == 'fy'){
                $query .= '&fq=fiscal_year_id:'.$year_value;
            }elseif($year_type == 'cy'){
                $query .= '&fq=calendar_fiscal_year_id:'.$year_value;
            }
        }
        $matches = _checkbook_autocomplete_solr_results($query, 'civil_service_title');
        drupal_json_output($matches);
    }
}

//Contracts autocomplete menu callback functions
/**
 * Construct query string and get results from Solr.
 *
 * @param string $facet
 *   Facet results needed
 * @param string $query
 *   Partially constructed query
 * @param null $status
 *   Contract status
 * @param string|null $category
 *   Contract category
 * @param string|null $contract_type
 *   Agreement type code
 * @param string|null $agency
 *   Agency code
 * @param string|null $award_method
 *   Award method code
 * @param string|null $year
 *   Year
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 *
 * @return array
 *   Results from Solr
 */
function _get_contracts_advanced_search_autocomplete_results($facet, $query, $status = null, $category = null, $contract_type = null, $agency = null, $award_method = null, $year = null, $mwbe_category = null, $data_source = 'checkbook') {

    if($category){
        $query .= '&fq=contract_category_name:'.$category;
    }
    if ($agency) {
        $field = $data_source == 'checkbook_oge' ? 'oge_agency_id' : 'agency_id';
        $query .= '&fq='.$field.':'.$agency;
    }
    if($award_method){
        $code_id_array = _checkbook_advanced_search_autocomplete_get_code_id($award_method);
        $query .= '&fq=award_method_id:'.$code_id_array['id'];
    }
    if(!in_array($contract_type, array('No Contract Type Selected', '0'))){
        $code_id_array = _checkbook_advanced_search_autocomplete_get_code_id($contract_type);
        if($status == 'P'){
            $query .= '&fq=contract_type_code:'.$code_id_array['code'];
        }else{
            $query .= '&fq=contract_type_id:'.$code_id_array['id'];
        }
    }
    if($status == 'P'){
        $query .= '&fq=contract_status:pending';
    }
    if($status == 'A'){
        if(isset($year))
            $query .= _checkbook_autocomplete_get_start_end_dates($year, 'contracts', 'A');
        $query .= '&fq=contract_status:registered';
    }
    if($status == 'R'){
        $query .= '&fq=contract_status:registered';
        if(isset($year)){
            $query .= _checkbook_autocomplete_get_start_end_dates($year, 'contracts', 'R');
        }
    }
    if ($mwbe_category) {
        $query .= '&fq=minority_type_id:'._checkbook_advanced_search_autocomplete_get_mwbe_category($mwbe_category);
    }
    $matches = _checkbook_autocomplete_solr_results($query, $facet);
    return $matches;
}

/**
 * Get autocomplete results for vendor from Solr.
 *
 * @param string $status
 *   Status
 * @param string $category
 *   Contract category
 * @param string $contract_type
 *   Contract Type
 * @param string $agency
 *   Agency code
 * @param string $award_method
 *   Award method id
 * @param string $year
 *   Year
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 */
function _checkbook_advanced_search_autocomplete_contracts_vendor_name($status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category,  $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if(!empty($searchTerm)){
        $query = 'select/?q=vendor_name_autocomplete:'.$searchTerm.'&facet=true&facet.field=vendor_name&fq=domain:contracts&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_contracts_advanced_search_autocomplete_results('vendor_name', $query, $status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source);
        drupal_json_output($matches);
    }
}

/**
 * Get autocomplete results for Contract Status from Solr.
 *
 * @param string $status
 *   Status
 * @param string $category
 *   Contract category
 * @param string $contract_type
 *   Contract Type
 * @param string $agency
 *   Agency code
 * @param string $award_method
 *   Award method id
 * @param string $year
 *   Year
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 */
function _checkbook_advanced_search_autocomplete_contracts_contract_number($status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if(!empty($searchTerm)){
        $query = 'select/?q=contract_number:'.$searchTerm.'&facet=true&facet.field=contract_number&fq=domain:contracts&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_contracts_advanced_search_autocomplete_results('contract_number', $query, $status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source);
        drupal_json_output($matches);
    }
}

/**
 * Get autocomplete results for APT PIN from Solr.
 *
 * @param string $status
 *   Status
 * @param string $category
 *   Contract category
 * @param string $contract_type
 *   Contract Type
 * @param string $agency
 *   Agency code
 * @param string $award_method
 *   Award method id
 * @param string $year
 *   Year
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 */
function _checkbook_advanced_search_autocomplete_contracts_apt_pin($status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if(strlen($searchTerm) > 0){
        $query = 'select/?q=apt_pin:'. $searchTerm .'&facet=true&facet.field=apt_pin&fq=domain:contracts&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_contracts_advanced_search_autocomplete_results('apt_pin', $query, $status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source);
        drupal_json_output($matches);
    }
}

/**
 * Get autocomplete results for PIN from Solr.
 *
 * @param string $status
 *   Status
 * @param string $category
 *   Contract category
 * @param string $contract_type
 *   Contract Type
 * @param string $agency
 *   Agency code
 * @param string $award_method
 *   Award method id
 * @param string $year
 *   Year
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 */
function _checkbook_advanced_search_autocomplete_contracts_pin($status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if(strlen($searchTerm) > 0){
        $query = 'select/?q=pin:'. $searchTerm .'&facet=true&facet.field=pin&fq=domain:contracts&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_contracts_advanced_search_autocomplete_results('pin', $query, $status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source);
        drupal_json_output($matches);
    }
}

/**
 * Get autocomplete results for Entity Contract Number from Solr.
 *
 * @param string $status
 *   Status
 * @param string $category
 *   Contract category
 * @param string $contract_type
 *   Contract Type
 * @param string $agency
 *   Agency code
 * @param string $award_method
 *   Award method id
 * @param string $year
 *   Year
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 */
function _checkbook_advanced_search_autocomplete_contracts_entitycontractnum($status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    $facet_prefix = _get_autocomplete_search_term_facet_prefix();
    if(strlen($searchTerm) > 0){
        $query = 'select/?q=contract_entity_contract_number:'. $searchTerm .'&facet=true&facet.field=contract_entity_contract_number&facet.prefix='. $facet_prefix .'&fq=domain:contracts&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_contracts_advanced_search_autocomplete_results('contract_entity_contract_number', $query, $status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source);
        drupal_json_output($matches);
    }
}

/**
 * Get autocomplete results for Entity Commodity Line from Solr.
 *
 * @param string $status
 *   Status
 * @param string $category
 *   Contract category
 * @param string $contract_type
 *   Contract Type
 * @param string $agency
 *   Agency code
 * @param string $award_method
 *   Award method id
 * @param string $year
 *   Year
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 */
function _checkbook_advanced_search_autocomplete_contracts_commodityline($status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    $facet_prefix = _get_autocomplete_search_term_facet_prefix();
    if(strlen($searchTerm) > 0){
        $query = 'select/?q=contract_commodity_line:'. $searchTerm .'&facet=true&facet.field=contract_commodity_line&facet.prefix='. $facet_prefix .'&fq=domain:contracts&facet.limit=10&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_contracts_advanced_search_autocomplete_results('contract_commodity_line', $query, $status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source);
        drupal_json_output($matches);
    }
}

/**
 * Get autocomplete results for Budget Name from Solr.
 *
 * @param string $status
 *   Status
 * @param string $category
 *   Contract category
 * @param string $contract_type
 *   Contract Type
 * @param string $agency
 *   Agency code
 * @param string $award_method
 *   Award method id
 * @param string $year
 *   Year
 * @param $mwbe_category
 *   Selected M/WBE Category
 * @param string $data_source
 *   Data source (checkbook, checkbook_oge)
 */
function _checkbook_advanced_search_autocomplete_contracts_budgetname($status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source = 'checkbook') {
    $searchTerm = _get_autocomplete_search_term();
    if (!empty($searchTerm)) {
        $query = 'select/?q=contract_budget_name_autocomplete:' . $searchTerm . '&q.op=AND&rows=0&facet=true&facet.field=contract_budget_name&facet.mincount=1&facet.limit=50&wt=php';
        $query .= $data_source == 'checkbook_oge' ? '&fq=agency_type:oge' : '&fq=-agency_type:oge';
        $matches = _get_contracts_advanced_search_autocomplete_results('contract_budget_name', $query, $status, $category, $contract_type, $agency, $award_method, $year, $mwbe_category, $data_source);

        //Need to filter out fields in doc that should not be returned
        if($matches[0]['label'] == 'No Matches Found') {
            drupal_json_output($matches);
        }
        else {
            $searchTerm = trim(htmlspecialchars_decode($_REQUEST['term'], ENT_QUOTES));
            $unique_matches = array();
            $result_count = 0;
            foreach($matches as $key => $value) {
                if($result_count == 10) break;
                if (stripos(strtolower($value), strtolower($searchTerm)) !== false) {
                    $unique_matches[] = $matches[$key];
                    $result_count++;
                }
            }
            if($unique_matches)
                drupal_json_output($unique_matches);
            else
                drupal_json_output(array('label' => 'No Matches Found','value' => ''));
        }
    }
}

//Budget autocomplete menu callback functions

/** @return options for department based on selected agency and year */
function _checkbook_advanced_search_autocomplete_budget_department($fiscal_year, $agency) {
    $params =  array("agency_id"=>$agency,"budget_fiscal_year_id"=>$fiscal_year);
    $results = get_db_results(false, 'checkbook:budget', array("department_name.department_name"), $params,"department_name.department_name");
    if (count($results ) > 0) {
        $matches = array();
        foreach ($results as $key=>$value) {
            $matches[] = $value['department_name.department_name'];
        }
        drupal_json_output($matches);
    }
    else {
        drupal_json_output(array(array('label'=>'No Matches Found','value'=>'')));
    }
}

/** @return options for Expense Category based on selected agency, Department and year */
function _checkbook_advanced_search_autocomplete_budget_expcategory($fiscal_year, $agency, $dept) {
    $params =  array("agency_id"=>$agency,"budget_fiscal_year_id"=>$fiscal_year);
    if($dept != '0' ) {
        $params["department_name"] = str_replace('__','/',$dept);
    }
    $results = get_db_results(false, 'checkbook:budget', array("object_class_name.object_class_name"), $params,"object_class_name.object_class_name");
    if (count($results ) > 0) {
        $matches = array();
        foreach ($results as $key=>$value) {
            $matches[] = $value['object_class_name.object_class_name'];
        }
        drupal_json_output($matches);
    }
    else {
        drupal_json_output(array(array('label'=>'No Matches Found','value'=>'')));
    }
}

/** @return autosuggestions for budget code based on selected agency, department, expense category and year */
function _checkbook_advanced_search_autocomplete_budget_budgetcode($fiscal_year, $agency, $dept, $expcategory) {
    $searchTerm = _get_autocomplete_search_term();
    if (!empty($searchTerm)) {
        $query = 'select?q=budget_code_name_code_autocomplete:' . $searchTerm . '&facet=true&facet.field=budget_code_name_code&fq=domain:budget&facet.limit=10&wt=php';
        if ($fiscal_year) {
            $query .= '&fq=fiscal_year_id:' . $fiscal_year;
        }
        if ($agency) {
            $query .= '&fq=agency_id:' . $agency;
        }
        if ($dept) {
            $query .= '&fq=department_name:"' . _checkbook_autocomplete_escapeSolrValue(trim($dept)) .'"';
        }
        if ($expcategory) {
            $query .= '&fq=expenditure_object_name:"' . _checkbook_autocomplete_escapeSolrValue(trim($expcategory)) .'"';
        }
        $matches = _checkbook_autocomplete_solr_results($query, 'budget_code_name_code');
        drupal_json_output($matches);
    }
}

//Revenue autocomplete menu callback functions

/** @return auto suggestions for revenue class name based on selected year, fund class, agency, revenue category and budget year */
function _checkbook_advanced_search_autocomplete_revenue_revenueclass($year, $fundclass, $agency, $budgetyear, $revcat, $revsrc, $fundingsrc) {
    $searchTerm = _get_autocomplete_search_term();
    if (!empty($searchTerm)) {
        $query = 'select/?q=revenue_class_name_autocomplete:' . $searchTerm . '&facet=true&facet.field=revenue_class_name&fq=domain:revenue&facet.limit=10&wt=php';
        if($year) {
            $query .= '&fq=display_fiscal_year_id:' . $year;
        }
        if($fundclass) {
            $query .= '&fq=fund_class_id:' . $fundclass;
        }
        if($agency) {
            $query .= '&fq=agency_id:' . $agency;
        }
        if($budgetyear) {
            $query .= '&fq=fiscal_year_id:' . $budgetyear;
        }
        if($revcat) {
            $query .= '&fq=revenue_category_id:' . $revcat;
        }
        if ($fundingsrc) {
            $query .= '&fq=funding_class_code:' . $fundingsrc;
        }
        $matches = _checkbook_autocomplete_solr_results($query, 'revenue_class_name');
        drupal_json_output($matches);
    }
}

/** @return auto suggestions for revenue source name based on selected year, fund class, agency, revenue category and budget year */
function _checkbook_advanced_search_autocomplete_revenue_revenuesource($year, $fundclass, $agency, $budgetyear, $revcat, $revclass, $fundingsrc) {
    $searchTerm = _get_autocomplete_search_term();
    if (!empty($searchTerm)) {
        $query = 'select/?q=revenue_source_name_autocomplete:' . $searchTerm . '&facet=true&facet.field=revenue_source_name&fq=domain:revenue&facet.limit=10&wt=php';
        if ($year) {
            $query .= '&fq=display_fiscal_year_id:' . $year;
        }
        if ($fundclass) {
            $query .= '&fq=fund_class_id:' . $fundclass;
        }
        if ($agency) {
            $query .= '&fq=agency_id:' . $agency;
        }
        if ($budgetyear) {
            $query .= '&fq=fiscal_year_id:' . $budgetyear;
        }
        if ($revcat) {
            $query .= '&fq=revenue_category_id:' . $revcat;
        }
        if ($fundingsrc) {
            $query .= '&fq=funding_class_code:' . $fundingsrc;
        }
        $matches = _checkbook_autocomplete_solr_results($query, 'revenue_source_name');
        drupal_json_output($matches);
    }
}

/**
 * Used for Contracts Advanced Search, for some elements like "Contract Type" which is a drop down on Advanced Search, we need contract_type_code
 * if status is "Pending" and contract_type_id if status is "Active" / "Registered". So $code_id_string is always in the format "id=>13~code=>41".
 * From this we have to separate out id and code
 *
 * @param $code_id_string
 * @return array('code' => 41, 'id' => 13)
 *
 */

function _checkbook_advanced_search_autocomplete_get_code_id($code_id_string){
    $code_id_array = explode('~',$code_id_string);
    $id = substr($code_id_array[0],4);
    $code = substr($code_id_array[1],6);
    return array('code' => $code, 'id' => $id);
}

/**
 * Used for the M/WBE Category dropdown in the advanced search this may have a list
 *
 * For example $minority_types = 1~2~3, return (1 OR 2 OR 3)
 * @param $minority_types
 * @return string
 *
 */

function _checkbook_advanced_search_autocomplete_get_mwbe_category($minority_types){
    return '('.implode('%20OR%20',explode('~',$minority_types)).')';
}