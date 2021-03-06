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


if ( _getRequestParamValue("datasource") == "checkbook_oge") {
    $datasource ="/datasource/checkbook_oge";
}

//Main table header
$tbl['header']['title'] = "<h3>Spending by Expense Category</h3>";
$tbl['header']['columns'] = array(
    array('value' => WidgetUtil::generateLabelMappingNoDiv("expense_category"), 'type' => 'text'),
    array('value' => WidgetUtil::generateLabelMappingNoDiv("encumbered_amount"), 'type' => 'number'),
    array('value' => WidgetUtil::generateLabelMappingNoDiv("spent_to_date"), 'type' => 'number')
    );
//echo "<h3>Spending by Expense Category</h3>";
//echo '<table class="spending_by_expense_category">
//      <tr>
//        <th><span>EXPENSE CATEGORY</span></th>
//        <th><span>ENCUMBERED AMOUNT</span></th>
//        <th><span>SPENT TO DATE</span></th>
//      </tr>';

$count = 0;
if (count($node->data) > 0) {
    foreach ($node->data as $row) {

        $spending_link = "/spending/transactions/expcategorycode/" . $row['expenditure_object_code'] . "/contnum/" .
            $row['contract_number@checkbook:history_agreement/original_agreement_id@checkbook:aggregateon_contracts_expense']
            . $datasource . "/newwindow";

        if ($row['is_disbursements_exist'] == 'Y' && !preg_match("/newwindow/",current_path())) {
            $spent_to_date_value = custom_number_formatter_format((isset($row['spending_amount_disb']))?$row['spending_amount_disb']:$row['spending_amount'], 2, '$');
            $spent_to_date = "<a class=\"new_window\" href='" . $spending_link . "'>" . custom_number_formatter_format((isset($row['spending_amount_disb']))?$row['spending_amount_disb']:$row['spending_amount'], 2, '$') . "</a>";
        }
        else {
            $spent_to_date_value = custom_number_formatter_format($row['spending_amount'], 2, '$');
            $spent_to_date = custom_number_formatter_format($row['spending_amount'], 2, '$');
        }

//        echo '<tr>
//              <td><span>'.$row['expenditure_object_name'].'</span></td>
//              <td><span>'.custom_number_formatter_format($row['encumbered_amount'], 2, '$').'</span></td>
//              <td><span>'.$spent_to_date.'</span></td>
//              </tr>';

        //Main table columns
        $tbl['body']['rows'][$count]['columns'] = array(
            array('value' => $row['expenditure_object_name'], 'type' => 'text'),
            array('value' => custom_number_formatter_format($row['encumbered_amount'], 2, '$'), 'type' => 'number'),
            array('value' => $spent_to_date_value, 'type' => 'number_link', 'link_value' => $spent_to_date)
        );
        $count += 1;
    }
}

$html = WidgetUtil::generateTable($tbl);
echo $html;
?>