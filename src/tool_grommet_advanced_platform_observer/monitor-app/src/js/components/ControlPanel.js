import React, { Component, PropTypes } from 'react';
import Box from 'grommet/components/Box';
import Heading from 'grommet/components/Heading';
import List from 'grommet/components/List';
import ListItem from 'grommet/components/ListItem';
import Meter from 'grommet/components/Meter';
import Value from 'grommet/components/Value';
import Status from 'grommet/components/icons/Status';
import Form from 'grommet/components/Form';
import Header from 'grommet/components/Header';
import FormFields from 'grommet/components/FormFields';
import FormField from 'grommet/components/FormField';
import Footer from 'grommet/components/Footer';
import Button from 'grommet/components/Button';
import TextInput from 'grommet/components/TextInput';
import Columns from 'grommet/components/Columns';
import Tiles from 'grommet/components/Tiles';
import Layer from 'grommet/components/Layer';
import Tile from 'grommet/components/Tile';
import Title from 'grommet/components/Title';
import EditIcon from 'grommet/components/icons/base/Edit';
import SettingsOptionIcon from 'grommet/components/icons/base/SettingsOption';
import ResumeIcon from 'grommet/components/icons/base/Resume';




export default class ControlPanel extends Component {

  constructor () {
    super();
    this._choosepathToFiles = this._choosepathToFiles.bind(this);
    this.state = {
    };
  }

  _choosepathToFiles () {

  }

  render () {
//    this.setState({pathToFiles : this.props.pathToFiles});
    console.log("render runned in ControlPanel %o", this.state);
    return (


    <Header direction="row" justify="between"
      fixed={true}
      pad={{horizontal: 'medium'}}
      primary={true}
      colorIndex="neutral-4-t">
      <Box flex={true}
        direction="row"
        responsive={false}>
        <Title> Directory :
        </Title>
          <Box
          colorIndex="light-2"
          pad="small"
          margin="small"
          basis='2/3'>
            {this.props.pathToFiles }
          </Box>
          <Box colorIndex="light-2"
          style={{height: "80%"}}
          margin="small">
            <Button onClick={this.props.onClickEditPath}
            icon={<EditIcon />}/>
          </Box>

          <Box colorIndex="light-2"
          style={{height: "80%"}}
          margin="small">
            <Button onClick={this.props.onClickEditPath}
            icon={<ResumeIcon />}/>
          </Box>

          <Box colorIndex="light-2"
          style={{height: "80%"}}
          margin="small">
            <Button onClick={this.props.onClickEditPath}
            icon={<SettingsOptionIcon />}/>
          </Box>

      </Box>
    </Header>
    );
  }
};


  ControlPanel.propTypes = {
    pathToFiles: PropTypes.string.isRequired,
    onClickEditPath: PropTypes.func.isRequired
  }
