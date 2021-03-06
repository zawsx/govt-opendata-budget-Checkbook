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
?>
<?php
$records = $node->data;
if(is_array($records)){
    $row = $records[0];
    $noContr = WidgetUtil::getLabel("no_of_contracts");

    if(_getRequestParamValue('smnid') == 720) {
        $noContr = WidgetUtil::getLabel("num_sub_contracts");
        $mwbe_category = WidgetUtil::getLabel("mwbe_category").': '.MappingUtil::getMinorityCategoryById($row['minority_type_minority_type']);
    }
    if(_getRequestParamValue('smnid') == 725) {
        $noContr = WidgetUtil::getLabel("num_sub_contracts");
        $mwbe_category = WidgetUtil::getLabel("mwbe_category").': '.MappingUtil::getMinorityCategoryById($row['prime_minority_type_prime_minority_type']);
    }
    if(_getRequestParamValue('smnid') == 726 || _getRequestParamValue('smnid') == 727 || _getRequestParamValue('smnid') == 728 || _getRequestParamValue('smnid') == 729) {
        $noContr = WidgetUtil::getLabel("num_sub_contracts");
    }
    if(_getRequestParamValue('smnid') == 783) {
        $mwbe_category = WidgetUtil::getLabel("mwbe_category").': '.MappingUtil::getMinorityCategoryById($row['current_prime_minority_type_id']);
    }
    if(_getRequestParamValue('smnid') == 784) {
        $mwbe_category = WidgetUtil::getLabel("mwbe_category").': '.MappingUtil::getMinorityCategoryById($row['minority_type_minority_type']);
    }
    if(_getRequestParamValue('smnid') == 791) {
        $noContr = WidgetUtil::getLabel("no_of_contracts");
        $mwbe_category = WidgetUtil::getLabel("mwbe_category").': '.MappingUtil::getMinorityCategoryById($row['minority_type_minority_type']);
    }

    $originalAmount = custom_number_formatter_format($row['original_amount_sum'],2,'$');
    $currentAmount = custom_number_formatter_format($row['current_amount_sum'],2,'$');
    $spentToDateAmount = custom_number_formatter_format($row['spending_amount_sum'],2,'$');
    $spnttodt = WidgetUtil::getLabel("spent_to_date");
    $oamnt = WidgetUtil::getLabel("original_amount");
    $camnt = WidgetUtil::getLabel("current_amount");
    $vendor= WidgetUtil::getLabel("vendor_name");
    $totalContracts = number_format($row['total_contracts']);
$summaryContent =  <<<EOD
<div class="contract-details-heading">
	<div class="contract-id">
		<h2 class="contract-title">{$node->widgetConfig->summaryView->templateTitle}</h2>
		<div class="contract-id">{$row[$node->widgetConfig->summaryView->entityColumnName]}<br>
		    {$mwbe_category}
		</div>
	</div>
	<div class="dollar-amounts">
		<div class="spent-to-date">
			{$spentToDateAmount}
            <div class="amount-title">{$spnttodt}</div>
		</div>
		<div class="original-amount">
		    {$originalAmount}
            <div class="amount-title">{$oamnt}</div>
		</div>
		<div class="current-amount">
		    {$currentAmount}
            <div class="amount-title">{$camnt}</div>
		</div>
		<div class="no-of-contracts">
			{$totalContracts}
			<div class="amount-title">{$noContr}</div>
		</div>
	</div>
</div>
EOD;

print $summaryContent;

}