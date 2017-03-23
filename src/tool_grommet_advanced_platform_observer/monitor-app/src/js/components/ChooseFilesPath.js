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



export default class ChooseFilesPath extends Component {


  constructor () {
    super();
    this._choosepathToFiles = this._choosepathToFiles.bind(this);
    this._onPathChange = this._onPathChange.bind(this);
    this.state = {
      pathChoosed: undefined,
    };
  }

  _choosepathToFiles (e) {
    e.preventDefault();
    if(this.state.pathChoosed) { // Check if pathChoosed is not undefined
      this.props.onSubmit(
        this.state.pathChoosed
      );
    }
  }

  _onPathChange (e) {
    this.setState({pathChoosed: e.target.value});
  }

  render () {
      console.log("render runned in ChooseFilesPath  %o ", this.state);
    return (
      <Layer align="top"  closer={true} hidden={this.props.hideWindow} onClose={this.props.onClose}>

        <Form onSubmit={this._choosepathToFiles} fill={true} style={{width: "1000px"}}>
          <Tiles fill={true}>
            <Tile basis='full' pad="small">
              <FormField label="Path to monitoring files" htmlFor="patheToFilesId" >
                <TextInput type="text" name="pathToFiles" id="patheToFilesId" defaultValue={this.props.defaultText} onInput={this._onPathChange} style={{width: "100%"}} />
              </FormField>
            </Tile>
            <Tile  pad="small">
              <Button label="Validate"
                type="submit"
                fill={true}
                primary={true}
                onClick={this._choosepathToFiles}
              />
            </Tile>
          </Tiles>
        </Form>

      </Layer>

    );
  }
};

  ChooseFilesPath.propTypes = {
    onSubmit: PropTypes.func.isRequired,
    defaultText: PropTypes.string
  }
