from xml.etree.ElementTree import fromstring

# HTML = """
# <table class="details" border="0" cellpadding="5" cellspacing="2" width="95%">
#   <tr valign="top">
#     <th>Tests</th>
#     <th>Failures</th>
#     <th>Success Rate</th>
#     <th>Average Time</th>
#     <th>Min Time</th>
#     <th>Max Time</th>
#   </tr>
#   <tr valign="top" class="Failure">
#     <td>103</td>
#     <td>24</td>
#     <td>76.70%</td>
#     <td>71 ms</td>
#     <td>0 ms</td>
#     <td>829 ms</td>
#   </tr>
#   <tr valign="top" class="whatever">
#     <td>A</td>
#     <td>B</td>
#     <td>C</td>
#     <td>D</td>
#     <td>E</td>
#     <td>F</td>
#   </tr>
# </table>"""

HTML = """
<table class="table table-condensed table-striped">
            <tr>
                
                    <th>List</th>
                
                <th>Rank</th>
                <th>Site</th>
                <th>System</th>
                <th>Cores</th>
                
                    <th>Rmax (TFlop/s)</th>
                    <th>Rpeak (TFlop/s)</th>
                
                <th>Power (kW)</th>
            </tr>
            <tr>
                    

                        <td><span class="">11/2016</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50623">National Supercomputing Center in Wuxi</a><br>China</td>
                    <td><a href="/system/178764">
                        <b>Sunway TaihuLight</b> - Sunway MPP, Sunway SW26010 260C 1.45GHz, Sunway
                    </a><br>NRCPC</td>
                    <td>10,649,600</td>
                
                    <td>93,014.6</td>
                    <td>125,435.9</td>
                
                    <td>15,371</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2016</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50623">National Supercomputing Center in Wuxi</a><br>China</td>
                    <td><a href="/system/178764">
                        <b>Sunway TaihuLight</b> - Sunway MPP, Sunway SW26010 260C 1.45GHz, Sunway
                    </a><br>NRCPC</td>
                    <td>10,649,600</td>
                
                    <td>93,014.6</td>
                    <td>125,435.9</td>
                
                    <td>15,371</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2015</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50365">National Super Computer Center in Guangzhou</a><br>China</td>
                    <td><a href="/system/177999">
                        <b>Tianhe-2 (MilkyWay-2)</b> - TH-IVB-FEP Cluster, Intel Xeon E5-2692 12C 2.200GHz, TH Express-2, Intel Xeon Phi 31S1P
                    </a><br>NUDT</td>
                    <td>3,120,000</td>
                
                    <td>33,862.7</td>
                    <td>54,902.4</td>
                
                    <td>17,808</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2015</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50365">National Super Computer Center in Guangzhou</a><br>China</td>
                    <td><a href="/system/177999">
                        <b>Tianhe-2 (MilkyWay-2)</b> - TH-IVB-FEP Cluster, Intel Xeon E5-2692 12C 2.200GHz, TH Express-2, Intel Xeon Phi 31S1P
                    </a><br>NUDT</td>
                    <td>3,120,000</td>
                
                    <td>33,862.7</td>
                    <td>54,902.4</td>
                
                    <td>17,808</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2014</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50365">National Super Computer Center in Guangzhou</a><br>China</td>
                    <td><a href="/system/177999">
                        <b>Tianhe-2 (MilkyWay-2)</b> - TH-IVB-FEP Cluster, Intel Xeon E5-2692 12C 2.200GHz, TH Express-2, Intel Xeon Phi 31S1P
                    </a><br>NUDT</td>
                    <td>3,120,000</td>
                
                    <td>33,862.7</td>
                    <td>54,902.4</td>
                
                    <td>17,808</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2014</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50365">National Super Computer Center in Guangzhou</a><br>China</td>
                    <td><a href="/system/177999">
                        <b>Tianhe-2 (MilkyWay-2)</b> - TH-IVB-FEP Cluster, Intel Xeon E5-2692 12C 2.200GHz, TH Express-2, Intel Xeon Phi 31S1P
                    </a><br>NUDT</td>
                    <td>3,120,000</td>
                
                    <td>33,862.7</td>
                    <td>54,902.4</td>
                
                    <td>17,808</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2013</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50365">National Super Computer Center in Guangzhou</a><br>China</td>
                    <td><a href="/system/177999">
                        <b>Tianhe-2 (MilkyWay-2)</b> - TH-IVB-FEP Cluster, Intel Xeon E5-2692 12C 2.200GHz, TH Express-2, Intel Xeon Phi 31S1P
                    </a><br>NUDT</td>
                    <td>3,120,000</td>
                
                    <td>33,862.7</td>
                    <td>54,902.4</td>
                
                    <td>17,808</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2013</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50460">National University of Defense Technology</a><br>China</td>
                    <td><a href="/system/177999">
                        <b>Tianhe-2 (MilkyWay-2)</b> - TH-IVB-FEP Cluster, Intel Xeon E5-2692 12C 2.200GHz, TH Express-2, Intel Xeon Phi 31S1P
                    </a><br>NUDT</td>
                    <td>3,120,000</td>
                
                    <td>33,862.7</td>
                    <td>54,902.4</td>
                
                    <td>17,808</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2012</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48553">DOE/SC/Oak Ridge National Laboratory</a><br>United States</td>
                    <td><a href="/system/177975">
                        <b>Titan</b> - Cray XK7 , Opteron 6274 16C 2.200GHz, Cray Gemini interconnect, NVIDIA K20x
                    </a><br>Cray Inc.</td>
                    <td>560,640</td>
                
                    <td>17,590.0</td>
                    <td>27,112.5</td>
                
                    <td>8,209</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2012</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49763">DOE/NNSA/LLNL</a><br>United States</td>
                    <td><a href="/system/177556">
                        <b>Sequoia</b> - BlueGene/Q, Power BQC 16C 1.60 GHz, Custom
                    </a><br>IBM</td>
                    <td>1,572,864</td>
                
                    <td>16,324.8</td>
                    <td>20,132.7</td>
                
                    <td>7,890</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2011</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50313">RIKEN Advanced Institute for Computational Science (AICS)</a><br>Japan</td>
                    <td><a href="/system/177232">K computer, SPARC64 VIIIfx 2.0GHz, Tofu interconnect
                    </a><br>Fujitsu</td>
                    <td>705,024</td>
                
                    <td>10,510.0</td>
                    <td>11,280.4</td>
                
                    <td>12,659.9</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2011</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50313">RIKEN Advanced Institute for Computational Science (AICS)</a><br>Japan</td>
                    <td><a href="/system/177232">K computer, SPARC64 VIIIfx 2.0GHz, Tofu interconnect
                    </a><br>Fujitsu</td>
                    <td>548,352</td>
                
                    <td>8,162.0</td>
                    <td>8,773.6</td>
                
                    <td>9,898.6</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2010</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50310">National Supercomputing Center in Tianjin</a><br>China</td>
                    <td><a href="/system/176929">
                        <b>Tianhe-1A</b> - NUDT TH MPP, X5670 2.93Ghz 6C, NVIDIA GPU, FT-1000 8C
                    </a><br>NUDT</td>
                    <td>186,368</td>
                
                    <td>2,566.0</td>
                    <td>4,701.0</td>
                
                    <td>4,040</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2010</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48553">DOE/SC/Oak Ridge National Laboratory</a><br>United States</td>
                    <td><a href="/system/176544">
                        <b>Jaguar</b> - Cray XT5-HE Opteron 6-core 2.6 GHz
                    </a><br>Cray Inc.</td>
                    <td>224,162</td>
                
                    <td>1,759.0</td>
                    <td>2,331.0</td>
                
                    <td>6,950</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2009</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48553">DOE/SC/Oak Ridge National Laboratory</a><br>United States</td>
                    <td><a href="/system/176544">
                        <b>Jaguar</b> - Cray XT5-HE Opteron 6-core 2.6 GHz
                    </a><br>Cray Inc.</td>
                    <td>224,162</td>
                
                    <td>1,759.0</td>
                    <td>2,331.0</td>
                
                    <td>6,950</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2009</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50157">DOE/NNSA/LANL</a><br>United States</td>
                    <td><a href="/system/176028">
                        <b>Roadrunner</b> - BladeCenter QS22/LS21 Cluster, PowerXCell 8i 3.2 Ghz / Opteron DC 1.8 GHz, Voltaire Infiniband
                    </a><br>IBM</td>
                    <td>129,600</td>
                
                    <td>1,105.0</td>
                    <td>1,456.7</td>
                
                    <td>2,483</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2008</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50157">DOE/NNSA/LANL</a><br>United States</td>
                    <td><a href="/system/176028">
                        <b>Roadrunner</b> - BladeCenter QS22/LS21 Cluster, PowerXCell 8i 3.2 Ghz / Opteron DC 1.8 GHz, Voltaire Infiniband
                    </a><br>IBM</td>
                    <td>129,600</td>
                
                    <td>1,105.0</td>
                    <td>1,456.7</td>
                
                    <td>2,483</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2008</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/50157">DOE/NNSA/LANL</a><br>United States</td>
                    <td><a href="/system/176026">
                        <b>Roadrunner</b> - BladeCenter QS22/LS21 Cluster, PowerXCell 8i 3.2 Ghz / Opteron DC 1.8 GHz, Voltaire Infiniband
                    </a><br>IBM</td>
                    <td>122,400</td>
                
                    <td>1,026.0</td>
                    <td>1,375.8</td>
                
                    <td>2,345</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2007</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49763">DOE/NNSA/LLNL</a><br>United States</td>
                    <td><a href="/system/175171">
                        <b>BlueGene/L</b> - eServer Blue Gene Solution
                    </a><br>IBM</td>
                    <td>212,992</td>
                
                    <td>478.2</td>
                    <td>596.4</td>
                
                    <td>2,329</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2007</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49763">DOE/NNSA/LLNL</a><br>United States</td>
                    <td><a href="/system/174275">
                        <b>BlueGene/L</b> - eServer Blue Gene Solution
                    </a><br>IBM</td>
                    <td>131,072</td>
                
                    <td>280.6</td>
                    <td>367.0</td>
                
                    <td>1,433</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2006</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49763">DOE/NNSA/LLNL</a><br>United States</td>
                    <td><a href="/system/174275">
                        <b>BlueGene/L</b> - eServer Blue Gene Solution
                    </a><br>IBM</td>
                    <td>131,072</td>
                
                    <td>280.6</td>
                    <td>367.0</td>
                
                    <td>1,433</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2006</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49763">DOE/NNSA/LLNL</a><br>United States</td>
                    <td><a href="/system/174275">
                        <b>BlueGene/L</b> - eServer Blue Gene Solution
                    </a><br>IBM</td>
                    <td>131,072</td>
                
                    <td>280.6</td>
                    <td>367.0</td>
                
                    <td>1,433</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2005</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49763">DOE/NNSA/LLNL</a><br>United States</td>
                    <td><a href="/system/174275">
                        <b>BlueGene/L</b> - eServer Blue Gene Solution
                    </a><br>IBM</td>
                    <td>131,072</td>
                
                    <td>280.6</td>
                    <td>367.0</td>
                
                    <td>1,433</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2005</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49763">DOE/NNSA/LLNL</a><br>United States</td>
                    <td><a href="/system/174210">
                        <b>BlueGene/L</b> - eServer Blue Gene Solution
                    </a><br>IBM</td>
                    <td>65,536</td>
                
                    <td>136.8</td>
                    <td>183.5</td>
                
                    <td>716</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2004</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49752">IBM/DOE</a><br>United States</td>
                    <td><a href="/system/173632">
                        <b>BlueGene/L beta-System</b> - BlueGene/L DD2 beta-System (0.7 GHz PowerPC 440)
                    </a><br>IBM</td>
                    <td>32,768</td>
                
                    <td>70.7</td>
                    <td>91.8</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2004</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49318">Japan Agency for Marine -Earth Science and Technology</a><br>Japan</td>
                    <td><a href="/system/167148">Earth-Simulator
                    </a><br>NEC</td>
                    <td>5,120</td>
                
                    <td>35.9</td>
                    <td>41.0</td>
                
                    <td>3,200</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2003</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49318">Japan Agency for Marine -Earth Science and Technology</a><br>Japan</td>
                    <td><a href="/system/167148">Earth-Simulator
                    </a><br>NEC</td>
                    <td>5,120</td>
                
                    <td>35.9</td>
                    <td>41.0</td>
                
                    <td>3,200</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2003</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49318">Japan Agency for Marine -Earth Science and Technology</a><br>Japan</td>
                    <td><a href="/system/167148">Earth-Simulator
                    </a><br>NEC</td>
                    <td>5,120</td>
                
                    <td>35.9</td>
                    <td>41.0</td>
                
                    <td>3,200</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2002</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49318">Japan Agency for Marine -Earth Science and Technology</a><br>Japan</td>
                    <td><a href="/system/167148">Earth-Simulator
                    </a><br>NEC</td>
                    <td>5,120</td>
                
                    <td>35.9</td>
                    <td>41.0</td>
                
                    <td>3,200</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2002</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49318">Japan Agency for Marine -Earth Science and Technology</a><br>Japan</td>
                    <td><a href="/system/167148">Earth-Simulator
                    </a><br>NEC</td>
                    <td>5,120</td>
                
                    <td>35.9</td>
                    <td>41.0</td>
                
                    <td>3,200</td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2001</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48247">Lawrence Livermore National Laboratory</a><br>United States</td>
                    <td><a href="/system/172411">ASCI White, SP Power3 375 MHz
                    </a><br>IBM</td>
                    <td>8,192</td>
                
                    <td>7.2</td>
                    <td>12.3</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2001</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48247">Lawrence Livermore National Laboratory</a><br>United States</td>
                    <td><a href="/system/172411">ASCI White, SP Power3 375 MHz
                    </a><br>IBM</td>
                    <td>8,192</td>
                
                    <td>7.2</td>
                    <td>12.3</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/2000</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48247">Lawrence Livermore National Laboratory</a><br>United States</td>
                    <td><a href="/system/172411">ASCI White, SP Power3 375 MHz
                    </a><br>IBM</td>
                    <td>8,192</td>
                
                    <td>4.9</td>
                    <td>12.3</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/2000</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48748">Sandia National Laboratories</a><br>United States</td>
                    <td><a href="/system/168753">ASCI Red
                    </a><br>Intel</td>
                    <td>9,632</td>
                
                    <td>2.4</td>
                    <td>3.2</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/1999</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48748">Sandia National Laboratories</a><br>United States</td>
                    <td><a href="/system/168753">ASCI Red
                    </a><br>Intel</td>
                    <td>9,632</td>
                
                    <td>2.4</td>
                    <td>3.2</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/1999</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48748">Sandia National Laboratories</a><br>United States</td>
                    <td><a href="/system/168752">ASCI Red
                    </a><br>Intel</td>
                    <td>9,472</td>
                
                    <td>2.1</td>
                    <td>3.2</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/1998</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48748">Sandia National Laboratories</a><br>United States</td>
                    <td><a href="/system/168751">ASCI Red
                    </a><br>Intel</td>
                    <td>9,152</td>
                
                    <td>1.3</td>
                    <td>1.8</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/1998</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48748">Sandia National Laboratories</a><br>United States</td>
                    <td><a href="/system/168751">ASCI Red
                    </a><br>Intel</td>
                    <td>9,152</td>
                
                    <td>1.3</td>
                    <td>1.8</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/1997</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48748">Sandia National Laboratories</a><br>United States</td>
                    <td><a href="/system/168751">ASCI Red
                    </a><br>Intel</td>
                    <td>9,152</td>
                
                    <td>1.3</td>
                    <td>1.8</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/1997</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48748">Sandia National Laboratories</a><br>United States</td>
                    <td><a href="/system/168750">ASCI Red
                    </a><br>Intel</td>
                    <td>7,264</td>
                
                    <td>1.1</td>
                    <td>1.5</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/1996</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/47529">Center for Computational Sciences, University of Tsukuba</a><br>Japan</td>
                    <td><a href="/system/167078">CP-PACS/2048
                    </a><br>Hitachi/Tsukuba</td>
                    <td>2,048</td>
                
                    <td>0.4</td>
                    <td>0.6</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/1996</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/49205">University of Tokyo</a><br>Japan</td>
                    <td><a href="/system/170731">SR2201/1024
                    </a><br>Hitachi</td>
                    <td>1,024</td>
                
                    <td>0.2</td>
                    <td>0.3</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/1995</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48404">National Aerospace Laboratory of Japan</a><br>Japan</td>
                    <td><a href="/system/173279">Numerical Wind Tunnel
                    </a><br>Fujitsu</td>
                    <td>140</td>
                
                    <td>0.2</td>
                    <td>0.2</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/1995</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48404">National Aerospace Laboratory of Japan</a><br>Japan</td>
                    <td><a href="/system/173279">Numerical Wind Tunnel
                    </a><br>Fujitsu</td>
                    <td>140</td>
                
                    <td>0.2</td>
                    <td>0.2</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/1994</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48404">National Aerospace Laboratory of Japan</a><br>Japan</td>
                    <td><a href="/system/173279">Numerical Wind Tunnel
                    </a><br>Fujitsu</td>
                    <td>140</td>
                
                    <td>0.2</td>
                    <td>0.2</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/1994</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48748">Sandia National Laboratories</a><br>United States</td>
                    <td><a href="/system/172538">XP/S140
                    </a><br>Intel</td>
                    <td>3,680</td>
                
                    <td>0.1</td>
                    <td>0.2</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">11/1993</span></td>
                    
                    <td><span class="">1</span></td>
                    <td><a href="/site/48404">National Aerospace Laboratory of Japan</a><br>Japan</td>
                    <td><a href="/system/173279">Numerical Wind Tunnel
                    </a><br>Fujitsu</td>
                    <td>140</td>
                
                    <td>0.1</td>
                    <td>0.2</td>
                
                    <td></td>
                </tr>
            
                <tr>
                    

                        <td><span class="">06/1993</span></td>
                    <td><span class="">1</span></td>
                    <td><a href="/site/48276">Los Alamos National Laboratory</a><br>United States</td>
                    <td><a href="/system/166997">CM-5/1024
                    </a><br>Thinking Machines Corporation</td>
                    <td>1,024</td>
                
                    <td>0.1</td>
                    <td>0.1</td>
                
                    <td></td>
                </tr>       
        </table>
"""

tree = fromstring(HTML)
rows = tree.findall("tr")
headrow = rows[0]
datarows = rows[1:]

for num, h in enumerate(headrow):
    data = ", ".join([row[num].text for row in datarows])
    print "{0:<16}: {1}".format(h.text, data)