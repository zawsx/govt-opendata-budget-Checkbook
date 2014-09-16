<div id="chart_container_<?php echo $id; ?>" style="width: 300px; height: 50px">
</div>
<script>
jQuery(document).ready(function ($) {
    var chart = new Highcharts.Chart({
      chart: {
         renderTo: 'chart_container_<?php echo $id; ?>',
         defaultSeriesType: 'bar',
         margin: [10,10,10,10],
         height:50         
      },
      title: {
         text: null
      },
      exporting: {
         enabled: false
      },
      legend: {
          enabled: false
      },
      credits: {
          enabled: false
      },
      xAxis: {
        categories: ['Ethnicity'],
        lineWidth :0,
        tickWidth: 0,
        labels: {
          enabled: false
      	}
      },
      yAxis: {
         min: 0,
         gridLineWidth :0,
         title: {
            text: null
         },
         labels: {
             enabled: false
       	}
         
      },
      tooltip: {
         formatter: function() {
            return this.series.name +' - $'+this.y+' ('+ Math.round(this.percentage) +'%)';
         }
      },
      plotOptions: {
         series: {
            stacking: 'percent',
            borderWidth: 1,
            minPointLength: 0,
            pointWidth: 18,
            shadow: false            
         }
      },
      series: [
         {name: 'Individuals & Other',
          data: [<?php echo ($data_row['io_mwbe'])? $data_row['io_mwbe'] : null ; ?>],
          color: '#858f9b'
         },
         {name: 'Non-M/WBE',
          data: [<?php echo ($data_row['n_mwbe'])? $data_row['n_mwbe'] : null ; ?>],
          color: '#2e5a8b'
         },
         {name: 'Women',
          data: [<?php echo ($data_row['w_mwbe'])? $data_row['w_mwbe'] : null ; ?>],
          color: '#eb8e27'
         },
         {name: 'Hispanic American',
          data: [<?php echo ($data_row['ha_mwbe'])? $data_row['ha_mwbe'] : null ; ?>],
          color: '#9ab46a'
         }, 
         {name: 'Black American',
          data: [<?php echo ($data_row['ba_mwbe'])? $data_row['ba_mwbe'] : null ; ?>],
          color: '#7db7e5'
         },   
        {name: 'Asian American',
          data: [<?php echo ($data_row['aa_mwbe'])? $data_row['aa_mwbe'] : null ; ?>],
          color: '#b8d8ef'
         }
      ]
 });
});
</script>  